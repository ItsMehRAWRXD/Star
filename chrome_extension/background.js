const STORE = {
  async get() {
    return new Promise((resolve) => chrome.storage.sync.get([
      'openaiKey', 'openaiModel', 'githubToken', 'aggressiveMode'
    ], resolve));
  }
};

chrome.runtime.onInstalled.addListener(() => {
  chrome.contextMenus.create({ id: 'sla-analyze-selection', title: 'Analyze selection (OpenAI)', contexts: ['selection'] });
});

async function callOpenAI(prompt, key, model, temperature = 0.2) {
  const resp = await fetch('https://api.openai.com/v1/chat/completions', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
      'Authorization': `Bearer ${key}`
    },
    body: JSON.stringify({
      model: model || 'gpt-4o-mini',
      temperature: temperature, 
      messages: [
        { role: 'system', content: 'Provide concise, safe, benign assistance. Prefer C++/MASM for code tasks.' },
        { role: 'user', content: prompt }
      ]
    })
  });
  if (!resp.ok) throw new Error('OpenAI error: ' + resp.status);
  const j = await resp.json();
  return j.choices?.[0]?.message?.content || '';
}

async function githubSearchCode(q, token) {
  const url = `https://api.github.com/search/code?q=${encodeURIComponent(q)}&per_page=50`;
  const headers = { 'Accept': 'application/vnd.github+json' };
  if (token) headers['Authorization'] = `Bearer ${token}`;
  const resp = await fetch(url, { headers });
  if (!resp.ok) throw new Error('GitHub error: ' + resp.status);
  const j = await resp.json();
  return j.items || [];
}

chrome.contextMenus.onClicked.addListener(async (info, tab) => {
  if (info.menuItemId !== 'sla-analyze-selection' || !info.selectionText) return;
  const cfg = await STORE.get();
  if (!cfg.openaiKey) {
    chrome.notifications.create({ type: 'basic', title: 'Self-Learning Agent', iconUrl: 'icon128.png', message: 'Set OpenAI key in options.' });
    return;
  }
  try {
    const content = await callOpenAI(`Analyze the following code/text. Improve, explain, and provide C++/MASM examples if relevant.\n\n${info.selectionText}`, cfg.openaiKey, cfg.openaiModel);
    chrome.notifications.create({ type: 'basic', title: 'Analysis done', iconUrl: 'icon128.png', message: String(content).slice(0, 8000) });
  } catch (e) {
    chrome.notifications.create({ type: 'basic', title: 'Error', iconUrl: 'icon128.png', message: String(e) });
  }
});

// Long repo analysis (triggered from popup)
async function analyzeRepository(repoUrl, cfg, onProgress) {
  // Expect https://github.com/owner/repo
  const m = repoUrl.match(/^https:\/\/github.com\/([^\/]+)\/([^\/]+)\/?/);
  if (!m) throw new Error('Invalid GitHub repo URL');
  const [_, owner, repo] = m;
  // List via GitHub API v3: contents and trees
  const headers = { 'Accept': 'application/vnd.github+json' };
  if (cfg.githubToken) headers['Authorization'] = `Bearer ${cfg.githubToken}`;
  // Get default branch
  const repoResp = await fetch(`https://api.github.com/repos/${owner}/${repo}`, { headers });
  if (!repoResp.ok) throw new Error('Repo fetch failed: ' + repoResp.status);
  const repoInfo = await repoResp.json();
  const sha = repoInfo.default_branch;
  // Get tree recursive (max ~50k files; we will cap ~4000)
  const treeResp = await fetch(`https://api.github.com/repos/${owner}/${repo}/git/trees/${sha}?recursive=1`, { headers });
  if (!treeResp.ok) throw new Error('Tree fetch failed: ' + treeResp.status);
  const tree = await treeResp.json();
  const files = (tree.tree || []).filter(x => x.type === 'blob' && /\.(cpp|hpp|h|c|cc|asm|inc|masm)$/i.test(x.path)).slice(0, 4000);
  onProgress && onProgress(`Indexed ${files.length} files`);

  // Fetch file contents in chunks
  const chunks = [];
  let batch = [];
  const maxBatch = cfg.aggressiveMode ? 50 : 25;
  for (let i = 0; i < files.length; i++) {
    batch.push(files[i]);
    if (batch.length === maxBatch || i === files.length - 1) {
      const texts = await Promise.all(batch.map(async f => {
        const r = await fetch(`https://raw.githubusercontent.com/${owner}/${repo}/${sha}/${encodeURI(f.path)}`);
        if (!r.ok) return `// ${f.path}: failed`;
        const t = await r.text();
        return `// ${f.path}\n${t}`;
      }));
      chunks.push(texts.join('\n\n'));
      onProgress && onProgress(`Fetched ${Math.min(i+1, files.length)}/${files.length}`);
      batch = [];
    }
  }

  // Summarize chunks with OpenAI
  const summaries = [];
  const temp = cfg.aggressiveMode ? 0.0 : 0.2;
  const conc = cfg.aggressiveMode ? 4 : 1;
  for (let i = 0; i < chunks.length; i += conc) {
    const slice = chunks.slice(i, i + conc);
    const results = await Promise.all(slice.map((chunk) => callOpenAI(`Summarize the following code. Extract components, responsibilities, and any build details. Prefer C++/MASM naming.\n\n${chunk.slice(0, 120000)}`, cfg.openaiKey, cfg.openaiModel, temp)));
    for (const r of results) summaries.push(r);
    onProgress && onProgress(`Summarized ${Math.min(i+conc, chunks.length)}/${chunks.length}`);
  }

  // Global synthesis
  const finalReport = await callOpenAI(`Combine these summaries into a single high-level architecture and task completion plan. Provide concrete steps and code sketches in C++ and MASM where appropriate.\n\n${summaries.join('\n\n---\n\n')}`, cfg.openaiKey, cfg.openaiModel);
  return finalReport;
}

// Expose to popup
chrome.runtime.onMessage.addListener((msg, sender, sendResponse) => {
  (async () => {
    if (msg?.type === 'analyzeRepo') {
      const cfg = await STORE.get();
      try {
        const report = await analyzeRepository(msg.repoUrl, cfg, (p) => chrome.runtime.sendMessage({ type: 'progress', message: p }));
        sendResponse({ ok: true, report });
      } catch (e) {
        sendResponse({ ok: false, error: String(e) });
      }
      return;
    }
  })();
  return true; // async
});