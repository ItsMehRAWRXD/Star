const HOST_NAME = "com.self_learning_agent";

chrome.runtime.onInstalled.addListener(() => {
  chrome.contextMenus.create({
    id: "sla-send",
    title: "Learn task with Self-Learning Agent",
    contexts: ["selection"]
  });
});

chrome.contextMenus.onClicked.addListener(async (info, tab) => {
  if (info.menuItemId !== "sla-send" || !info.selectionText) return;
  try {
    const port = chrome.runtime.connectNative(HOST_NAME);
    const payload = { task: info.selectionText, policy: "bandit", maxSteps: 8 };
    port.postMessage(payload);
    port.onMessage.addListener((msg) => {
      const text = (msg && msg.output) ? String(msg.output).slice(0, 8000) : "No output";
      chrome.notifications?.create({
        type: "basic",
        title: "Self-Learning Agent",
        iconUrl: "icon128.png",
        message: text
      });
      port.disconnect();
    });
    port.onDisconnect.addListener(() => {});
  } catch (e) {
    console.error("Native host error", e);
  }
});