<template>
  <div class="container mx-auto px-4 py-6 bg-slate-50 dark:bg-slate-900 min-h-screen">
    <!-- 页面标题 -->
    <div class="flex items-center justify-between mb-6 pb-4 border-b border-slate-200 dark:border-slate-700">
      <div class="flex items-center">
        <div :class="wifiEnabled ? 'text-green-500' : 'text-red-500' mr-3">
          <i class="fas fa-wifi text-2xl"></i>
        </div>
        <h1 class="text-xl font-bold text-slate-800 dark:text-white">WiFi管理</h1>
      </div>
      <div class="text-sm text-slate-500 dark:text-slate-400" id="refresh-timer">实时更新中</div>
    </div>

    <!-- WiFi状态信息区域 -->
    <div class="bg-white dark:bg-slate-800 rounded-xl shadow-md p-5 mb-6 transition-all duration-300 hover:shadow-lg">
      <h2 class="text-lg font-semibold text-slate-800 dark:text-white mb-4 flex items-center">
        <i class="fas fa-info-circle text-blue-500 mr-2"></i>WiFi状态
      </h2>
      
      <div class="grid grid-cols-1 md:grid-cols-2 gap-4">
        <div class="flex items-center justify-between p-3 bg-slate-50 dark:bg-slate-700/50 rounded-lg">
          <span class="text-slate-600 dark:text-slate-300">WiFi状态：</span>
          <span :class="wifiEnabled ? 'text-green-600 dark:text-green-400 font-medium' : 'text-red-600 dark:text-red-400 font-medium'">
            {{ wifiEnabled ? '已启用' : '已禁用' }}
          </span>
        </div>
        
        <div class="flex items-center justify-between p-3 bg-slate-50 dark:bg-slate-700/50 rounded-lg">
          <span class="text-slate-600 dark:text-slate-300">WiFi频段：</span>
          <span class="text-purple-600 dark:text-purple-400 font-medium">{{ wifiMode }}</span>
        </div>
        
        <div class="flex items-center justify-between p-3 bg-slate-50 dark:bg-slate-700/50 rounded-lg">
          <span class="text-slate-600 dark:text-slate-300">WiFi名称：</span>
          <span class="text-slate-800 dark:text-white font-medium">{{ wifiSsid || '--' }}</span>
        </div>
        
        <div class="flex items-center justify-between p-3 bg-slate-50 dark:bg-slate-700/50 rounded-lg">
          <span class="text-slate-600 dark:text-slate-300">WiFi密码：</span>
          <div class="flex items-center">
            <span class="text-slate-800 dark:text-white font-medium mr-2">
              {{ showPassword ? wifiKey : '••••••••' }}
            </span>
            <button @click="togglePasswordVisibility" class="text-slate-400 hover:text-slate-600 dark:hover:text-slate-200">
              <i :class="showPassword ? 'fas fa-eye-slash' : 'fas fa-eye'"></i>
            </button>
          </div>
        </div>
        
        <div class="flex items-center justify-between p-3 bg-slate-50 dark:bg-slate-700/50 rounded-lg">
          <span class="text-slate-600 dark:text-slate-300">当前信道：</span>
          <span class="text-slate-800 dark:text-white font-medium">{{ wifiChannel || '--' }}</span>
        </div>
        
        <div class="flex items-center justify-between p-3 bg-slate-50 dark:bg-slate-700/50 rounded-lg">
          <span class="text-slate-600 dark:text-slate-300">加密方式：</span>
          <span class="text-slate-800 dark:text-white font-medium">{{ wifiEncryption || '--' }}</span>
        </div>
      </div>
    </div>

    <!-- WiFi设置区域 -->
    <div class="bg-white dark:bg-slate-800 rounded-xl shadow-md p-5 transition-all duration-300 hover:shadow-lg">
      <h2 class="text-lg font-semibold text-slate-800 dark:text-white mb-4 flex items-center">
        <i class="fas fa-cog text-blue-500 mr-2"></i>WiFi设置
      </h2>
      
      <div class="space-y-5">
        <!-- WiFi开关 -->
        <div class="flex items-center justify-between p-3 bg-slate-50 dark:bg-slate-700/50 rounded-lg">
          <div>
            <span class="text-slate-600 dark:text-slate-300">WiFi状态：</span>
            <p class="text-xs text-slate-500 dark:text-slate-400 mt-1">开启或关闭WiFi功能</p>
          </div>
          <label class="relative cursor-pointer">
            <input type="checkbox" v-model="wifiEnabled" @change="toggleWifi" class="sr-only peer">
            <div class="w-14 h-7 bg-slate-300 dark:bg-slate-600 rounded-full peer peer-checked:bg-green-500 transition-colors"></div>
            <div class="absolute top-0.5 left-0.5 w-6 h-6 bg-white rounded-full shadow transition-transform peer-checked:translate-x-7"></div>
          </label>
        </div>
        
        <!-- 频段切换 -->
        <div class="flex items-center justify-between p-3 bg-slate-50 dark:bg-slate-700/50 rounded-lg">
          <div>
            <span class="text-slate-600 dark:text-slate-300">WiFi频段：</span>
            <p class="text-xs text-slate-500 dark:text-slate-400 mt-1">选择2.4G或5G频段</p>
          </div>
          <label class="relative cursor-pointer">
            <input type="checkbox" v-model="is5G" @change="switchWifiMode" class="sr-only peer">
            <div class="w-24 h-7 bg-slate-300 dark:bg-slate-600 rounded-full peer peer-checked:bg-purple-500 transition-colors"></div>
            <div class="absolute top-0.5 left-0.5 w-6 h-6 bg-white rounded-full shadow transition-transform peer-checked:translate-x-17"></div>
            <div class="absolute inset-0 flex items-center justify-between px-2 pointer-events-none">
              <span class="text-xs font-medium text-white">2.4G</span>
              <span class="text-xs font-medium text-white">5G</span>
            </div>
          </label>
        </div>
        
        <!-- SSID设置 -->
        <div class="p-3 bg-slate-50 dark:bg-slate-700/50 rounded-lg">
          <label class="block text-slate-600 dark:text-slate-300 mb-2">WiFi名称</label>
          <div class="flex">
            <input 
              type="text" 
              v-model="ssidInput" 
              class="flex-1 px-3 py-2 border border-slate-300 dark:border-slate-600 rounded-l-lg bg-white dark:bg-slate-800 text-slate-800 dark:text-white focus:outline-none focus:ring-2 focus:ring-blue-500"
              placeholder="输入WiFi名称"
            >
          </div>
        </div>
        
        <!-- 密码设置 -->
        <div class="p-3 bg-slate-50 dark:bg-slate-700/50 rounded-lg">
          <label class="block text-slate-600 dark:text-slate-300 mb-2">WiFi密码</label>
          <div class="flex">
            <input 
              :type="inputType" 
              v-model="passwordInput" 
              class="flex-1 px-3 py-2 border border-slate-300 dark:border-slate-600 rounded-l-lg bg-white dark:bg-slate-800 text-slate-800 dark:text-white focus:outline-none focus:ring-2 focus:ring-blue-500"
              placeholder="输入WiFi密码（至少8位）"
              @focus="inputType = 'text'"
              @blur="inputType = 'password'"
            >
            <button 
              @click="saveWifiSettings"
              class="bg-green-500 hover:bg-green-600 text-white px-4 py-2 rounded-r-lg transition-colors"
            >
              保存
            </button>
          </div>
        </div>
        
        <!-- 信道设置 -->
        <div class="p-3 bg-slate-50 dark:bg-slate-700/50 rounded-lg">
          <label class="block text-slate-600 dark:text-slate-300 mb-2">WiFi信道</label>
          <div class="flex">
            <select 
              v-model="selectedChannel" 
              class="flex-1 px-3 py-2 border border-slate-300 dark:border-slate-600 rounded-l-lg bg-white dark:bg-slate-800 text-slate-800 dark:text-white focus:outline-none focus:ring-2 focus:ring-blue-500"
            >
              <option v-for="channel in availableChannels" :key="channel" :value="channel">{{ channel }}</option>
            </select>
            <button 
              @click="saveWifiChannel"
              class="bg-blue-500 hover:bg-blue-600 text-white px-4 py-2 rounded-r-lg transition-colors"
            >
              保存
            </button>
          </div>
        </div>
      </div>
    </div>

    <!-- 已连接设备 -->
    <div class="mt-6 bg-white dark:bg-slate-800 rounded-xl shadow-md p-5 transition-all duration-300 hover:shadow-lg">
      <h2 class="text-lg font-semibold text-slate-800 dark:text-white mb-4 flex items-center">
        <i class="fas fa-users text-blue-500 mr-2"></i>已连接设备
      </h2>
      
      <div v-if="clients.length === 0" class="text-center py-6 text-slate-500 dark:text-slate-400">
        <i class="fas fa-wifi-slash text-4xl mb-2 opacity-30"></i>
        <p>暂无设备连接</p>
      </div>
      
      <div class="space-y-3" v-else>
        <div v-for="client in clients" :key="client.mac" class="flex items-center justify-between p-3 bg-slate-50 dark:bg-slate-700/50 rounded-lg">
          <div>
            <p class="font-medium text-slate-800 dark:text-white">{{ client.name || '未知设备' }}</p>
            <p class="text-xs text-slate-500 dark:text-slate-400">{{ client.mac }}</p>
          </div>
          <div class="text-right">
            <p class="text-sm text-slate-800 dark:text-white">{{ client.ip }}</p>
            <button 
              @click="kickClient(client.mac)"
              class="text-red-500 hover:text-red-600 text-xs mt-1"
            >
              踢出
            </button>
          </div>
        </div>
      </div>
    </div>

    <!-- 提示消息 -->
    <div :class="['fixed top-4 right-4 px-4 py-3 rounded-lg shadow-lg z-50 transition-all duration-300 transform', toast.show ? 'translate-y-0 opacity-100' : '-translate-y-10 opacity-0', toast.type === 'error' ? 'bg-red-500' : 'bg-green-500']">
      <p class="text-white font-medium">{{ toast.message }}</p>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, watch } from 'vue';

// 状态变量
const wifiEnabled = ref(true);
const wifiMode = ref('2.4G');
const is5G = ref(false);
const wifiSsid = ref('');
const wifiKey = ref('');
const showPassword = ref(false);
const wifiChannel = ref('');
const wifiEncryption = ref('');
const clients = ref([]);

// 设置变量
const ssidInput = ref('');
const passwordInput = ref('');
const inputType = ref('password');
const selectedChannel = ref('');
const availableChannels = ref([]);

// 提示消息
const toast = ref({
  show: false,
  message: '',
  type: ''
});

// 信道选项
const channels24G = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13];
const channels5G = [36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144, 149, 153, 157, 161, 165];

// 定时器ID
let refreshIntervalId = null;

// 加载WiFi数据
const loadWifiData = async () => {
  try {
    const response = await fetch('/api/wifi/config');
    if (!response.ok) throw new Error('获取WiFi信息失败');
    
    const data = await response.json();
    
    // 更新状态
    wifiEnabled.value = data.wifi_enabled === 'on';
    wifiMode.value = data.wifi_mode || '2.4G';
    is5G.value = wifiMode.value === '5G';
    wifiSsid.value = data.wifi_ssid || '';
    wifiKey.value = data.wifi_key || '';
    wifiChannel.value = data.wifi_channel || '';
    wifiEncryption.value = data.wifi_encryption || '';
    
    // 更新设置区域（仅在用户未编辑时）
    if (!document.activeElement || !['INPUT', 'SELECT'].includes(document.activeElement.tagName)) {
      ssidInput.value = wifiSsid.value;
      if (!passwordInput.value) {
        passwordInput.value = wifiKey.value;
      }
      selectedChannel.value = wifiChannel.value;
    }
    
    // 更新可用信道
    availableChannels.value = is5G.value ? channels5G : channels24G;
    
    // 更新连接的客户端
    await loadWifiClients();
    
    // 更新刷新时间
    updateRefreshTimer();
  } catch (error) {
    console.error('加载WiFi数据失败:', error);
    document.getElementById('refresh-timer').textContent = '更新失败';
  }
};

// 加载连接的客户端
const loadWifiClients = async () => {
  try {
    const response = await fetch('/api/wifi/clients');
    if (!response.ok) throw new Error('获取客户端列表失败');
    
    const data = await response.json();
    clients.value = data.clients || [];
  } catch (error) {
    console.error('加载客户端失败:', error);
  }
};

// 更新刷新计时器显示
const updateRefreshTimer = () => {
  document.getElementById('refresh-timer').textContent = '实时更新中';
};

// 切换密码显示状态
const togglePasswordVisibility = () => {
  showPassword.value = !showPassword.value;
};

// 切换WiFi开关
const toggleWifi = async () => {
  try {
    const response = await fetch('/api/wifi/config', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({
        action: 'toggle_wifi',
        enabled: wifiEnabled.value
      })
    });
    
    if (!response.ok) throw new Error('操作失败');
    
    const data = await response.json();
    showToast(data.message || '操作成功');
  } catch (error) {
    // 操作失败时还原状态
    wifiEnabled.value = !wifiEnabled.value;
    showToast('操作失败: ' + error.message, 'error');
  }
};

// 切换WiFi模式
const switchWifiMode = async () => {
  try {
    const targetMode = is5G.value ? '5G' : '2.4G';
    
    const response = await fetch('/api/wifi/config', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({
        action: 'switch_wifi_mode',
        target_mode: targetMode
      })
    });
    
    if (!response.ok) throw new Error('切换失败');
    
    const data = await response.json();
    wifiMode.value = targetMode;
    availableChannels.value = is5G.value ? channels5G : channels24G;
    showToast(data.message || '频段切换成功');
    
    // 重新加载数据
    setTimeout(loadWifiData, 1000);
  } catch (error) {
    // 操作失败时还原状态
    is5G.value = !is5G.value;
    showToast('切换失败: ' + error.message, 'error');
  }
};

// 保存WiFi设置
const saveWifiSettings = async () => {
  try {
    const ssid = ssidInput.value.trim();
    const password = passwordInput.value;
    
    if (!ssid) {
      showToast('请输入网络名称', 'error');
      return;
    }
    
    if (password && password.length < 8) {
      showToast('密码长度至少8位', 'error');
      return;
    }
    
    const response = await fetch('/api/wifi/config', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({
        action: 'update_wifi_settings',
        ssid,
        key: password
      })
    });
    
    if (!response.ok) throw new Error('设置失败');
    
    const data = await response.json();
    showToast(data.message || 'WiFi设置已更新');
    
    // 重新加载数据
    setTimeout(loadWifiData, 1000);
  } catch (error) {
    showToast('设置失败: ' + error.message, 'error');
  }
};

// 保存WiFi信道
const saveWifiChannel = async () => {
  try {
    if (!selectedChannel.value) {
      showToast('请选择信道', 'error');
      return;
    }
    
    const response = await fetch('/api/wifi/config', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({
        action: 'switch_wifi_channel',
        target_channel: selectedChannel.value
      })
    });
    
    if (!response.ok) throw new Error('信道设置失败');
    
    const data = await response.json();
    showToast(data.message || '信道设置成功');
    
    // 重新加载数据
    setTimeout(loadWifiData, 1000);
  } catch (error) {
    showToast('信道设置失败: ' + error.message, 'error');
  }
};

// 踢出客户端
const kickClient = async (mac) => {
  try {
    const response = await fetch('/api/wifi/clients', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({
        action: 'kick',
        mac
      })
    });
    
    if (!response.ok) throw new Error('踢出失败');
    
    const data = await response.json();
    showToast(data.message || '已成功踢出设备');
    
    // 重新加载客户端列表
    setTimeout(loadWifiClients, 1000);
  } catch (error) {
    showToast('踢出失败: ' + error.message, 'error');
  }
};

// 显示提示消息
const showToast = (message, type = 'success') => {
  toast.value = {
    show: true,
    message,
    type
  };
  
  setTimeout(() => {
    toast.value.show = false;
  }, 3000);
};

// 监听频段变化，更新可用信道
watch(is5G, (newVal) => {
  availableChannels.value = newVal ? channels5G : channels24G;
});

// 页面加载时初始化
onMounted(() => {
  // 加载WiFi数据
  loadWifiData();
  
  // 设置定时器，每2秒刷新一次
  refreshIntervalId = setInterval(loadWifiData, 2000);
  
  // 页面离开时清除定时器
  return () => {
    if (refreshIntervalId) {
      clearInterval(refreshIntervalId);
    }
  };
});
</script>

<style scoped>
/* 基础动画效果 */
.fade-enter-active, .fade-leave-active {
  transition: opacity 0.3s;
}
.fade-enter, .fade-leave-to {
  opacity: 0;
}
</style>
