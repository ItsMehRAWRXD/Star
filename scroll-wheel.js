/**
 * VSCode.dev Scroll Wheel PoC
 * Interactive scroll wheel component with smooth animations
 */

class ScrollWheel {
    constructor(containerId) {
        this.container = document.getElementById(containerId);
        this.currentIndex = 0;
        this.scrollEventCount = 0;
        this.isAnimating = false;
        this.items = [
            { label: 'Files', description: 'Manage your project files and folder structure', icon: '📁' },
            { label: 'Search', description: 'Find and replace text across your entire workspace', icon: '🔍' },
            { label: 'Debug', description: 'Debug your code with breakpoints and variable inspection', icon: '🔧' },
            { label: 'Settings', description: 'Customize your development environment preferences', icon: '⚙️' },
            { label: 'Analytics', description: 'View project statistics and code metrics', icon: '📊' },
            { label: 'Themes', description: 'Change the appearance and color scheme', icon: '🎨' },
            { label: 'Extensions', description: 'Install and manage powerful extensions', icon: '🧩' },
            { label: 'Tips', description: 'Learn useful tips and keyboard shortcuts', icon: '💡' }
        ];
        
        this.init();
    }
    
    init() {
        this.setupEventListeners();
        this.updateDisplay();
        this.updateStats();
        
        // Make the wheel focusable for keyboard navigation
        this.container.setAttribute('tabindex', '0');
        
        console.log('ScrollWheel initialized with', this.items.length, 'items');
    }
    
    setupEventListeners() {
        // Mouse wheel events
        this.container.addEventListener('wheel', (e) => this.handleWheel(e));
        
        // Click events on wheel items
        const wheelItems = this.container.querySelectorAll('.wheel-item');
        wheelItems.forEach((item, index) => {
            item.addEventListener('click', () => this.selectItem(index));
        });
        
        // Control button events
        const prevBtn = document.getElementById('prevBtn');
        const nextBtn = document.getElementById('nextBtn');
        
        if (prevBtn) prevBtn.addEventListener('click', () => this.previous());
        if (nextBtn) nextBtn.addEventListener('click', () => this.next());
        
        // Keyboard navigation
        this.container.addEventListener('keydown', (e) => this.handleKeyboard(e));
        
        // Focus events
        this.container.addEventListener('focus', () => this.onFocus());
        this.container.addEventListener('blur', () => this.onBlur());
        
        // Touch events for mobile
        this.setupTouchEvents();
        
        // Prevent context menu on right click
        this.container.addEventListener('contextmenu', (e) => e.preventDefault());
    }
    
    setupTouchEvents() {
        let touchStartY = 0;
        let touchStartX = 0;
        
        this.container.addEventListener('touchstart', (e) => {
            touchStartY = e.touches[0].clientY;
            touchStartX = e.touches[0].clientX;
        }, { passive: true });
        
        this.container.addEventListener('touchmove', (e) => {
            e.preventDefault();
        }, { passive: false });
        
        this.container.addEventListener('touchend', (e) => {
            const touchEndY = e.changedTouches[0].clientY;
            const touchEndX = e.changedTouches[0].clientX;
            const deltaY = touchStartY - touchEndY;
            const deltaX = touchStartX - touchEndX;
            
            // Determine if it's a vertical or horizontal swipe
            if (Math.abs(deltaY) > Math.abs(deltaX)) {
                if (Math.abs(deltaY) > 50) { // Minimum swipe distance
                    if (deltaY > 0) {
                        this.next();
                    } else {
                        this.previous();
                    }
                }
            }
        }, { passive: true });
    }
    
    handleWheel(e) {
        e.preventDefault();
        
        if (this.isAnimating) return;
        
        this.scrollEventCount++;
        this.updateStats();
        
        // Normalize wheel delta across different browsers
        const delta = Math.sign(e.deltaY);
        
        if (delta > 0) {
            this.next();
        } else {
            this.previous();
        }
        
        // Add visual feedback
        this.addScrollFeedback();
    }
    
    handleKeyboard(e) {
        if (this.isAnimating) return;
        
        switch (e.key) {
            case 'ArrowUp':
            case 'ArrowLeft':
                e.preventDefault();
                this.previous();
                break;
            case 'ArrowDown':
            case 'ArrowRight':
                e.preventDefault();
                this.next();
                break;
            case 'Home':
                e.preventDefault();
                this.selectItem(0);
                break;
            case 'End':
                e.preventDefault();
                this.selectItem(this.items.length - 1);
                break;
            case 'Enter':
            case ' ':
                e.preventDefault();
                this.activateCurrentItem();
                break;
        }
    }
    
    next() {
        if (this.isAnimating) return;
        
        const nextIndex = (this.currentIndex + 1) % this.items.length;
        this.selectItem(nextIndex);
    }
    
    previous() {
        if (this.isAnimating) return;
        
        const prevIndex = (this.currentIndex - 1 + this.items.length) % this.items.length;
        this.selectItem(prevIndex);
    }
    
    selectItem(index) {
        if (this.isAnimating || index === this.currentIndex) return;
        
        this.isAnimating = true;
        this.currentIndex = index;
        
        // Add rotation animation
        this.addRotationAnimation();
        
        // Update UI after animation delay
        setTimeout(() => {
            this.updateDisplay();
            this.updateStats();
            this.isAnimating = false;
        }, 200);
        
        // Provide haptic feedback if available
        this.triggerHapticFeedback();
    }
    
    updateDisplay() {
        // Update active item styling
        const wheelItems = this.container.querySelectorAll('.wheel-item');
        wheelItems.forEach((item, index) => {
            item.classList.toggle('active', index === this.currentIndex);
        });
        
        // Update selected item display
        const selectedItem = document.getElementById('selectedItem');
        const itemDescription = document.getElementById('itemDescription');
        
        if (selectedItem) {
            selectedItem.textContent = this.items[this.currentIndex].label;
        }
        
        if (itemDescription) {
            itemDescription.textContent = this.items[this.currentIndex].description;
        }
        
        // Update wheel indicator
        const indicator = this.container.querySelector('.wheel-indicator');
        if (indicator) {
            indicator.classList.add('active');
            setTimeout(() => indicator.classList.remove('active'), 1000);
        }
    }
    
    updateStats() {
        const currentPosition = document.getElementById('currentPosition');
        const totalItems = document.getElementById('totalItems');
        const scrollEvents = document.getElementById('scrollEvents');
        
        if (currentPosition) currentPosition.textContent = this.currentIndex;
        if (totalItems) totalItems.textContent = this.items.length;
        if (scrollEvents) scrollEvents.textContent = this.scrollEventCount;
    }
    
    addRotationAnimation() {
        this.container.classList.add('scrolling');
        setTimeout(() => {
            this.container.classList.remove('scrolling');
        }, 600);
    }
    
    addScrollFeedback() {
        // Add a temporary visual effect
        const feedback = document.createElement('div');
        feedback.style.cssText = `
            position: absolute;
            top: 50%;
            left: 50%;
            transform: translate(-50%, -50%);
            width: 40px;
            height: 40px;
            background: rgba(0, 122, 204, 0.3);
            border-radius: 50%;
            pointer-events: none;
            animation: feedbackPulse 0.3s ease-out;
        `;
        
        this.container.appendChild(feedback);
        
        setTimeout(() => {
            if (feedback.parentNode) {
                feedback.parentNode.removeChild(feedback);
            }
        }, 300);
    }
    
    triggerHapticFeedback() {
        // Trigger haptic feedback on supported devices
        if ('vibrate' in navigator) {
            navigator.vibrate(10);
        }
    }
    
    activateCurrentItem() {
        const currentItem = this.items[this.currentIndex];
        console.log('Activated item:', currentItem.label);
        
        // Add activation animation
        const wheelItem = this.container.querySelector(`[data-value="${this.currentIndex}"]`);
        if (wheelItem) {
            wheelItem.style.transform = 'scale(1.3)';
            setTimeout(() => {
                wheelItem.style.transform = '';
            }, 200);
        }
        
        // You could emit a custom event here for external handling
        const event = new CustomEvent('wheelItemActivated', {
            detail: {
                index: this.currentIndex,
                item: currentItem
            }
        });
        this.container.dispatchEvent(event);
    }
    
    onFocus() {
        this.container.style.boxShadow = '0 0 30px rgba(0, 122, 204, 0.5), inset 0 0 20px rgba(0, 0, 0, 0.3)';
    }
    
    onBlur() {
        this.container.style.boxShadow = '0 0 20px rgba(0, 122, 204, 0.3), inset 0 0 20px rgba(0, 0, 0, 0.3)';
    }
    
    // Public API methods
    getCurrentItem() {
        return this.items[this.currentIndex];
    }
    
    setItems(newItems) {
        this.items = newItems;
        this.currentIndex = Math.min(this.currentIndex, newItems.length - 1);
        this.updateDisplay();
        this.updateStats();
    }
    
    reset() {
        this.currentIndex = 0;
        this.scrollEventCount = 0;
        this.updateDisplay();
        this.updateStats();
    }
}

// Add feedback animation styles
const style = document.createElement('style');
style.textContent = `
    @keyframes feedbackPulse {
        0% {
            transform: translate(-50%, -50%) scale(0);
            opacity: 1;
        }
        100% {
            transform: translate(-50%, -50%) scale(1);
            opacity: 0;
        }
    }
`;
document.head.appendChild(style);

// Initialize the scroll wheel when DOM is loaded
document.addEventListener('DOMContentLoaded', () => {
    const scrollWheel = new ScrollWheel('scrollWheel');
    
    // Make it globally accessible for debugging
    window.scrollWheel = scrollWheel;
    
    // Listen for custom events
    document.getElementById('scrollWheel').addEventListener('wheelItemActivated', (e) => {
        console.log('Wheel item activated:', e.detail);
        
        // You could integrate with other parts of your application here
        // For example, navigate to different sections, open modals, etc.
    });
    
    // Add some demo functionality
    let demoMode = false;
    
    // Demo mode toggle (hidden feature)
    document.addEventListener('keydown', (e) => {
        if (e.key === 'd' && e.ctrlKey && e.shiftKey) {
            demoMode = !demoMode;
            if (demoMode) {
                startDemo();
            } else {
                stopDemo();
            }
        }
    });
    
    let demoInterval;
    
    function startDemo() {
        console.log('Demo mode started - automatic rotation every 2 seconds');
        demoInterval = setInterval(() => {
            scrollWheel.next();
        }, 2000);
    }
    
    function stopDemo() {
        console.log('Demo mode stopped');
        clearInterval(demoInterval);
    }
    
    // Add performance monitoring
    let frameCount = 0;
    let lastTime = performance.now();
    
    function measurePerformance() {
        frameCount++;
        const currentTime = performance.now();
        
        if (currentTime - lastTime >= 1000) {
            console.log(`FPS: ${frameCount}`);
            frameCount = 0;
            lastTime = currentTime;
        }
        
        requestAnimationFrame(measurePerformance);
    }
    
    // Start performance monitoring in development
    if (window.location.hostname === 'localhost' || window.location.hostname === '127.0.0.1') {
        measurePerformance();
    }
    
    console.log('VSCode.dev Scroll Wheel PoC initialized successfully!');
    console.log('Tips:');
    console.log('- Use mouse wheel or arrow keys to navigate');
    console.log('- Press Ctrl+Shift+D to toggle demo mode');
    console.log('- Access scrollWheel object from window.scrollWheel for debugging');
});