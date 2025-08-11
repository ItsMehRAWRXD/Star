# vscore.dev — Scroll Wheel PoC

A minimal, framework-free proof-of-concept for a circular scroll wheel component. Rotate via mouse wheel/trackpad, drag, or arrow keys. Snaps to the nearest item.

## Run locally

- Option 1: Open `index-inline.html` directly in a browser (single file, no server needed).
- Option 2: Open `index.html` directly in a browser.
- Option 3: Serve the folder (recommended for correct fonts):

```bash
npx --yes serve /workspace/poc-scroll-wheel -l 5173
```

Then open http://localhost:5173

## Files
- `index.html`: Markup
- `styles.css`: Visuals
- `script.js`: Interaction logic

## Integrate
Extract the `wheel-wrapper` block and the logic from `script.js`, wire it to your routing labels, and restyle via CSS variables.