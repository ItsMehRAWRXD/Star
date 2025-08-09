const HOST_NAME = "com.self_learning_agent";

const $ = (sel) => document.querySelector(sel);
$('#run').addEventListener('click', async () => {
  const task = $('#task').value.trim();
  const policy = $('#policy').value;
  const maxSteps = Math.max(1, Math.min(30, parseInt($('#steps').value || '8', 10)));
  if (!task) { $('#out').textContent = 'Please enter a task.'; return; }
  try {
    const port = chrome.runtime.connectNative(HOST_NAME);
    const payload = { task, policy, maxSteps };
    port.postMessage(payload);
    port.onMessage.addListener((msg) => {
      $('#out').textContent = (msg && msg.output) ? String(msg.output) : 'No output';
      port.disconnect();
    });
    port.onDisconnect.addListener(() => {});
  } catch (e) {
    $('#out').textContent = 'Native host error: ' + e;
  }
});