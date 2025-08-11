# VSCore Scroll Wheel Component - Proof of Concept

A modern, smooth scroll wheel implementation for vscore.dev with visual feedback and intuitive interactions.

## Features

- **Smooth Scrolling**: Butter-smooth animations using requestAnimationFrame
- **Multiple Input Methods**:
  - Mouse wheel scrolling
  - Click and drag the thumb
  - Click on track to jump to position
  - Keyboard navigation (Arrow keys, Page Up/Down, Home/End)
  - Touch support for mobile devices
- **Visual Feedback**:
  - Direction indicators (up/down arrows)
  - Real-time stats display (position, velocity, direction)
  - Pulsing effect at scroll bounds
  - Hover effects on track
- **Responsive Design**: Adapts to different screen sizes
- **Performance Optimized**: 60fps scrolling experience
- **Customizable**: Easy to modify colors, sizes, and behavior

## Quick Start

1. Open `index.html` in a modern web browser
2. Use any of the following methods to scroll:
   - Mouse wheel over the content area
   - Click and drag the blue thumb
   - Click anywhere on the track
   - Use keyboard keys:
     - ↑/↓ Arrow keys for small steps
     - Page Up/Down for larger jumps
     - Home/End to jump to start/end

## File Structure

```
vscore-scroll-wheel-poc/
├── index.html      # Main HTML structure
├── styles.css      # Styling and animations
├── script.js       # JavaScript functionality
└── README.md       # This file
```

## Integration

To integrate this scroll wheel into your project:

1. **HTML Structure**:
```html
<div class="scroll-wheel" id="scrollWheel">
    <div class="wheel-track">
        <div class="wheel-thumb" id="wheelThumb"></div>
    </div>
    <div class="wheel-indicators">
        <div class="indicator top">▲</div>
        <div class="indicator bottom">▼</div>
    </div>
</div>
```

2. **Initialize the Component**:
```javascript
const scrollWheel = new ScrollWheel();
```

3. **Customize via CSS Variables**:
```css
:root {
    --primary-color: #0066ff;
    --secondary-color: #00a8ff;
    --wheel-width: 60px;
    --thumb-size: 40px;
    --transition-speed: 0.3s;
}
```

## Customization Options

### JavaScript Configuration

The `ScrollWheel` class accepts several configuration options:

- `scrollSpeed`: Mouse wheel scroll sensitivity (default: 0.5)
- `dragSpeed`: Drag interaction speed (default: 1)
- `smoothness`: Smoothing factor for animations (default: 0.15)
- `velocityDamping`: How quickly velocity decreases (default: 0.95)

### CSS Customization

All visual aspects can be customized through CSS variables defined in `:root`:

- Colors: `--primary-color`, `--secondary-color`
- Sizes: `--wheel-width`, `--thumb-size`, `--track-width`
- Animation: `--transition-speed`
- Layout: `--border-radius`

## Browser Support

- Chrome/Edge: Full support
- Firefox: Full support
- Safari: Full support
- Mobile browsers: Touch-enabled support

## Performance

- Uses `requestAnimationFrame` for smooth 60fps animations
- Efficient event handling with throttling
- Minimal DOM manipulations
- CSS transforms for hardware acceleration

## Future Enhancements

- [ ] Momentum scrolling
- [ ] Custom easing functions
- [ ] Scroll snapping to sections
- [ ] Horizontal scroll support
- [ ] Mini-map preview
- [ ] Theme presets

## License

This is a proof of concept for vscore.dev. Feel free to use and modify as needed.