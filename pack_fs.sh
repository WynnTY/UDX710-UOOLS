#!/bin/bash
set -e

# 前端构建产物目录
DIST_DIR="web/dist"
# 目标 C 文件（后端用于嵌入静态文件）
OUTPUT_FILE="src/packed_fs.c"

# 检查 dist 目录是否存在
if [ ! -d "$DIST_DIR" ]; then
    echo "错误：未找到前端构建产物，请先执行 'cd web && npm run build'"
    exit 1
fi

# 生成 packed_fs.c 的头部内容
cat > "$OUTPUT_FILE" << 'EOF'
/**
 * @file packed_fs.c
 * @brief 自动生成的静态文件嵌入代码（由 pack_fs.sh 生成）
 */

#include <string.h>
#include "mongoose.h"

/* 静态文件数据结构：文件名 + 内容 + 长度 */
typedef struct {
    const char *path;
    const unsigned char *data;
    size_t len;
} packed_file_t;

EOF

# 遍历 dist 目录下的所有文件，生成 C 数组
find "$DIST_DIR" -type f | while read -r file; do
    # 计算文件相对路径（例如 dist/index.html → /index.html）
    rel_path="/${file#$DIST_DIR/}"
    # 生成变量名（替换特殊字符为下划线，例如 /index.html → index_html）
    var_name=$(echo "$rel_path" | sed 's/[^a-zA-Z0-9]/_/g' | sed 's/^_//')
    
    echo "处理文件: $rel_path → $var_name"
    
    # 追加文件数据到 packed_fs.c（使用 xxd 转换为 C 数组）
    echo "static const unsigned char ${var_name}_data[] = {" >> "$OUTPUT_FILE"
    xxd -i "$file" | sed 's/^.*{/{/' >> "$OUTPUT_FILE"  # 移除 xxd 自动生成的变量名
    echo "};" >> "$OUTPUT_FILE"
    echo "static const size_t ${var_name}_len = sizeof(${var_name}_data);" >> "$OUTPUT_FILE"
    echo "" >> "$OUTPUT_FILE"
done

# 生成文件索引数组（供 mongoose 查找文件）
cat >> "$OUTPUT_FILE" << 'EOF'
/* 静态文件索引表 */
static const packed_file_t s_packed_files[] = {
EOF

# 遍历文件，生成索引表条目
find "$DIST_DIR" -type f | while read -r file; do
    rel_path="/${file#$DIST_DIR/}"
    var_name=$(echo "$rel_path" | sed 's/[^a-zA-Z0-9]/_/g' | sed 's/^_//')
    echo "    {\"$rel_path\", ${var_name}_data, ${var_name}_len}," >> "$OUTPUT_FILE"
done

# 生成文件服务逻辑
cat >> "$OUTPUT_FILE" << 'EOF'
    {NULL, NULL, 0}  /* 结束标记 */
};

/* 静态文件服务配置 */
static struct mg_http_serve_opts s_opts = {
    .root_dir = NULL,  /* 不使用目录，直接从内存读取 */
    .ssi_pattern = NULL,
    .extra_headers = "Cache-Control: max-age=3600\r\n"
                     "Access-Control-Allow-Origin: *\r\n"
};

/**
 * 从内存中提供静态文件
 */
int serve_packed_file(struct mg_connection *c, struct mg_http_message *hm) {
    /* 查找匹配的文件 */
    for (const packed_file_t *f = s_packed_files; f->path != NULL; f++) {
        if (mg_http_match_uri(hm, f->path)) {
            mg_http_serve_static(c, hm, f->data, f->len, &s_opts);
            return 1;
        }
    }

    /* 根路径或SPA路由，默认返回 index.html */
    if (strcmp(hm->uri.buf, "/") == 0 || (strstr(hm->uri.buf, ".") == NULL && strncmp(hm->uri.buf, "/api/", 5) != 0)) {
        for (const packed_file_t *f = s_packed_files; f->path != NULL; f++) {
            if (strcmp(f->path, "/index.html") == 0) {
                mg_http_serve_static(c, hm, f->data, f->len, &s_opts);
                return 1;
            }
        }
    }

    return 0;  /* 未找到文件 */
}
EOF

echo "成功生成 $OUTPUT_FILE"
