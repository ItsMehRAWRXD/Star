// VSCore Scroll Wheel Component
class ScrollWheel {
    constructor() {
        // DOM Elements
        this.scrollWheel = document.getElementById('scrollWheel');
        this.wheelThumb = document.getElementById('wheelThumb');
        this.wheelTrack = this.scrollWheel.querySelector('.wheel-track');
        this.contentArea = document.getElementById('contentArea');
        this.contentWrapper = document.getElementById('contentWrapper');
        this.topIndicator = this.scrollWheel.querySelector('.indicator.top');
        this.bottomIndicator = this.scrollWheel.querySelector('.indicator.bottom');
        
        // Stats elements
        this.scrollPositionDisplay = document.getElementById('scrollPosition');
        this.scrollVelocityDisplay = document.getElementById('scrollVelocity');
        this.scrollDirectionDisplay = document.getElementById('scrollDirection');
        
        // State
        this.isDragging = false;
        this.currentPosition = 0;
        this.targetPosition = 0;
        this.velocity = 0;
        this.lastScrollTime = Date.now();
        this.lastScrollPosition = 0;
        this.scrollDirection = 'none';
        this.animationId = null;
        
        // Configuration
        this.scrollSpeed = 0.5;
        this.dragSpeed = 1;
        this.smoothness = 0.15;
        this.velocityDamping = 0.95;
        
        // Initialize
        this.init();
    }
    
    init() {
        // Calculate content dimensions
        this.updateDimensions();
        
        // Set up event listeners
        this.setupEventListeners();
        
        // Start animation loop
        this.animate();
        
        // Initial position
        this.updateThumbPosition(0);
        
        // Observe content sections
        this.observeSections();
    }
    
    updateDimensions() {
        this.contentHeight = this.contentWrapper.scrollHeight;
        this.viewportHeight = this.contentArea.clientHeight;
        this.scrollableHeight = this.contentHeight - this.viewportHeight;
        this.trackHeight = this.wheelTrack.clientHeight - this.wheelThumb.clientHeight;
        
        // Update thumb size based on content ratio
        const thumbHeightRatio = this.viewportHeight / this.contentHeight;
        const minThumbHeight = 40;
        const maxThumbHeight = this.wheelTrack.clientHeight * 0.5;
        const thumbHeight = Math.max(minThumbHeight, Math.min(maxThumbHeight, this.wheelTrack.clientHeight * thumbHeightRatio));
        this.wheelThumb.style.height = thumbHeight + 'px';
        
        // Recalculate track height
        this.trackHeight = this.wheelTrack.clientHeight - thumbHeight;
    }
    
    setupEventListeners() {
        // Mouse wheel events
        this.contentArea.addEventListener('wheel', this.handleWheel.bind(this), { passive: false });
        
        // Drag events
        this.wheelThumb.addEventListener('mousedown', this.startDrag.bind(this));
        document.addEventListener('mousemove', this.drag.bind(this));
        document.addEventListener('mouseup', this.endDrag.bind(this));
        
        // Track click
        this.wheelTrack.addEventListener('click', this.handleTrackClick.bind(this));
        
        // Touch events for mobile
        this.wheelThumb.addEventListener('touchstart', this.startDrag.bind(this));
        document.addEventListener('touchmove', this.drag.bind(this));
        document.addEventListener('touchend', this.endDrag.bind(this));
        
        // Window resize
        window.addEventListener('resize', () => {
            this.updateDimensions();
            this.updateThumbPosition(this.currentPosition);
        });
        
        // Keyboard navigation
        document.addEventListener('keydown', this.handleKeyboard.bind(this));
    }
    
    handleWheel(e) {
        e.preventDefault();
        
        // Calculate scroll delta
        const delta = e.deltaY * this.scrollSpeed;
        
        // Update target position
        this.targetPosition = Math.max(0, Math.min(this.scrollableHeight, this.targetPosition + delta));
        
        // Calculate velocity
        const currentTime = Date.now();
        const timeDelta = currentTime - this.lastScrollTime;
        const positionDelta = this.targetPosition - this.lastScrollPosition;
        
        if (timeDelta > 0) {
            this.velocity = positionDelta / timeDelta * 100;
            this.scrollDirection = positionDelta > 0 ? 'down' : 'up';
        }
        
        this.lastScrollTime = currentTime;
        this.lastScrollPosition = this.targetPosition;
        
        // Update indicators
        this.updateIndicators();
    }
    
    startDrag(e) {
        this.isDragging = true;
        this.wheelThumb.classList.add('dragging');
        
        const clientY = e.type.includes('touch') ? e.touches[0].clientY : e.clientY;
        this.dragStartY = clientY;
        this.dragStartPosition = this.currentPosition;
        
        e.preventDefault();
    }
    
    drag(e) {
        if (!this.isDragging) return;
        
        const clientY = e.type.includes('touch') ? e.touches[0].clientY : e.clientY;
        const deltaY = clientY - this.dragStartY;
        const scrollDelta = (deltaY / this.trackHeight) * this.scrollableHeight;
        
        this.targetPosition = Math.max(0, Math.min(this.scrollableHeight, this.dragStartPosition + scrollDelta));
        
        // Calculate drag velocity
        this.velocity = deltaY * this.dragSpeed;
        this.scrollDirection = deltaY > 0 ? 'down' : 'up';
    }
    
    endDrag() {
        this.isDragging = false;
        this.wheelThumb.classList.remove('dragging');
    }
    
    handleTrackClick(e) {
        if (e.target === this.wheelThumb) return;
        
        const rect = this.wheelTrack.getBoundingClientRect();
        const clickY = e.clientY - rect.top;
        const thumbHeight = this.wheelThumb.clientHeight;
        const clickPosition = (clickY - thumbHeight / 2) / this.trackHeight;
        
        this.targetPosition = clickPosition * this.scrollableHeight;
        this.targetPosition = Math.max(0, Math.min(this.scrollableHeight, this.targetPosition));
    }
    
    handleKeyboard(e) {
        const step = 50; // Pixels to scroll per key press
        
        switch(e.key) {
            case 'ArrowUp':
                e.preventDefault();
                this.targetPosition = Math.max(0, this.targetPosition - step);
                this.scrollDirection = 'up';
                break;
            case 'ArrowDown':
                e.preventDefault();
                this.targetPosition = Math.min(this.scrollableHeight, this.targetPosition + step);
                this.scrollDirection = 'down';
                break;
            case 'PageUp':
                e.preventDefault();
                this.targetPosition = Math.max(0, this.targetPosition - this.viewportHeight * 0.8);
                this.scrollDirection = 'up';
                break;
            case 'PageDown':
                e.preventDefault();
                this.targetPosition = Math.min(this.scrollableHeight, this.targetPosition + this.viewportHeight * 0.8);
                this.scrollDirection = 'down';
                break;
            case 'Home':
                e.preventDefault();
                this.targetPosition = 0;
                this.scrollDirection = 'up';
                break;
            case 'End':
                e.preventDefault();
                this.targetPosition = this.scrollableHeight;
                this.scrollDirection = 'down';
                break;
        }
        
        if (['ArrowUp', 'ArrowDown', 'PageUp', 'PageDown', 'Home', 'End'].includes(e.key)) {
            this.velocity = 100;
            this.updateIndicators();
        }
    }
    
    updateIndicators() {
        // Update direction indicators
        if (this.scrollDirection === 'up') {
            this.topIndicator.classList.add('active');
            this.bottomIndicator.classList.remove('active');
        } else if (this.scrollDirection === 'down') {
            this.topIndicator.classList.remove('active');
            this.bottomIndicator.classList.add('active');
        }
        
        // Remove active class after a delay
        setTimeout(() => {
            this.topIndicator.classList.remove('active');
            this.bottomIndicator.classList.remove('active');
        }, 300);
    }
    
    updateThumbPosition(position) {
        const thumbPosition = (position / this.scrollableHeight) * this.trackHeight;
        this.wheelThumb.style.top = thumbPosition + 'px';
    }
    
    updateContentPosition(position) {
        this.contentWrapper.style.transform = `translateY(${-position}px)`;
    }
    
    updateStats() {
        // Update scroll position percentage
        const percentage = this.scrollableHeight > 0 
            ? Math.round((this.currentPosition / this.scrollableHeight) * 100) 
            : 0;
        this.scrollPositionDisplay.textContent = percentage + '%';
        
        // Update velocity
        const displayVelocity = Math.round(Math.abs(this.velocity));
        this.scrollVelocityDisplay.textContent = displayVelocity;
        
        // Update direction
        this.scrollDirectionDisplay.textContent = this.scrollDirection === 'none' ? '-' : this.scrollDirection;
    }
    
    observeSections() {
        const options = {
            root: this.contentArea,
            rootMargin: '-20% 0px -20% 0px',
            threshold: 0.5
        };
        
        const observer = new IntersectionObserver((entries) => {
            entries.forEach(entry => {
                if (entry.isIntersecting) {
                    entry.target.classList.add('visible');
                } else {
                    entry.target.classList.remove('visible');
                }
            });
        }, options);
        
        const sections = this.contentWrapper.querySelectorAll('.content-section');
        sections.forEach(section => observer.observe(section));
    }
    
    animate() {
        // Smooth scrolling with easing
        if (!this.isDragging) {
            this.currentPosition += (this.targetPosition - this.currentPosition) * this.smoothness;
        } else {
            this.currentPosition = this.targetPosition;
        }
        
        // Apply velocity damping
        this.velocity *= this.velocityDamping;
        
        // Reset direction when stopped
        if (Math.abs(this.velocity) < 0.1) {
            this.scrollDirection = 'none';
        }
        
        // Update positions
        this.updateThumbPosition(this.currentPosition);
        this.updateContentPosition(this.currentPosition);
        
        // Update stats
        this.updateStats();
        
        // Add pulsing effect when at bounds
        if (this.currentPosition <= 1) {
            this.wheelThumb.classList.add('pulsing');
        } else if (this.currentPosition >= this.scrollableHeight - 1) {
            this.wheelThumb.classList.add('pulsing');
        } else {
            this.wheelThumb.classList.remove('pulsing');
        }
        
        // Continue animation
        this.animationId = requestAnimationFrame(this.animate.bind(this));
    }
}

// Initialize scroll wheel when DOM is loaded
document.addEventListener('DOMContentLoaded', () => {
    const scrollWheel = new ScrollWheel();
    
    // Expose to window for debugging
    window.scrollWheel = scrollWheel;
    
    // Add some demo functionality
    console.log('VSCore Scroll Wheel initialized');
    console.log('Use arrow keys, Page Up/Down, Home/End for keyboard navigation');
    console.log('Click and drag the wheel or use mouse wheel to scroll');
});