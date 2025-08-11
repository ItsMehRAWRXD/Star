class ScrollWheel {
    constructor() {
        this.wheel = document.getElementById('scrollWheel');
        this.wheelSegments = document.querySelector('.wheel-segments');
        this.wheelIndicator = document.getElementById('wheelIndicator');
        this.centerCircle = document.querySelector('.center-circle');
        
        // State variables
        this.currentRotation = 0;
        this.segmentCount = 8;
        this.wheelSize = 300;
        this.rotationSpeed = 1;
        this.isSpinning = false;
        this.lastScrollTime = 0;
        this.scrollDirection = 'none';
        this.activeSegment = null;
        
        // Control elements
        this.segmentCountSlider = document.getElementById('segmentCount');
        this.wheelSizeSlider = document.getElementById('wheelSize');
        this.rotationSpeedSlider = document.getElementById('rotationSpeed');
        this.resetBtn = document.getElementById('resetBtn');
        
        // Display elements
        this.segmentCountValue = document.getElementById('segmentCountValue');
        this.wheelSizeValue = document.getElementById('wheelSizeValue');
        this.rotationSpeedValue = document.getElementById('rotationSpeedValue');
        this.currentRotationDisplay = document.getElementById('currentRotation');
        this.activeSegmentDisplay = document.getElementById('activeSegment');
        this.scrollDirectionDisplay = document.getElementById('scrollDirection');
        
        this.init();
    }
    
    init() {
        this.createSegments();
        this.setupEventListeners();
        this.updateDisplay();
        this.updateCenterCircle();
    }
    
    createSegments() {
        this.wheelSegments.innerHTML = '';
        const segmentAngle = 360 / this.segmentCount;
        
        for (let i = 0; i < this.segmentCount; i++) {
            const segment = document.createElement('div');
            segment.className = 'segment';
            segment.textContent = `${i + 1}`;
            segment.style.transform = `rotate(${i * segmentAngle}deg)`;
            segment.dataset.index = i;
            
            // Add click event for segment selection
            segment.addEventListener('click', () => {
                this.selectSegment(i);
            });
            
            this.wheelSegments.appendChild(segment);
        }
    }
    
    setupEventListeners() {
        // Mouse wheel events
        this.wheel.addEventListener('wheel', (e) => {
            e.preventDefault();
            this.handleScroll(e.deltaY);
        });
        
        // Touch events for mobile
        let touchStartY = 0;
        let touchEndY = 0;
        
        this.wheel.addEventListener('touchstart', (e) => {
            touchStartY = e.touches[0].clientY;
        });
        
        this.wheel.addEventListener('touchmove', (e) => {
            e.preventDefault();
        });
        
        this.wheel.addEventListener('touchend', (e) => {
            touchEndY = e.changedTouches[0].clientY;
            const deltaY = touchStartY - touchEndY;
            this.handleScroll(deltaY);
        });
        
        // Control sliders
        this.segmentCountSlider.addEventListener('input', (e) => {
            this.segmentCount = parseInt(e.target.value);
            this.segmentCountValue.textContent = this.segmentCount;
            this.createSegments();
            this.updateActiveSegment();
        });
        
        this.wheelSizeSlider.addEventListener('input', (e) => {
            this.wheelSize = parseInt(e.target.value);
            this.wheelSizeValue.textContent = `${this.wheelSize}px`;
            this.wheel.style.width = `${this.wheelSize}px`;
            this.wheel.style.height = `${this.wheelSize}px`;
        });
        
        this.rotationSpeedSlider.addEventListener('input', (e) => {
            this.rotationSpeed = parseFloat(e.target.value);
            this.rotationSpeedValue.textContent = `${this.rotationSpeed.toFixed(1)}x`;
        });
        
        // Reset button
        this.resetBtn.addEventListener('click', () => {
            this.resetWheel();
        });
        
        // Keyboard events
        document.addEventListener('keydown', (e) => {
            switch(e.key) {
                case 'ArrowUp':
                    e.preventDefault();
                    this.handleScroll(-100);
                    break;
                case 'ArrowDown':
                    e.preventDefault();
                    this.handleScroll(100);
                    break;
                case ' ':
                    e.preventDefault();
                    this.toggleSpin();
                    break;
                case 'r':
                case 'R':
                    this.resetWheel();
                    break;
            }
        });
        
        // Mouse drag events for manual rotation
        let isDragging = false;
        let startAngle = 0;
        let startRotation = 0;
        
        this.wheel.addEventListener('mousedown', (e) => {
            isDragging = true;
            const rect = this.wheel.getBoundingClientRect();
            const centerX = rect.left + rect.width / 2;
            const centerY = rect.top + rect.height / 2;
            startAngle = Math.atan2(e.clientY - centerY, e.clientX - centerX);
            startRotation = this.currentRotation;
            this.wheel.style.cursor = 'grabbing';
        });
        
        document.addEventListener('mousemove', (e) => {
            if (!isDragging) return;
            
            const rect = this.wheel.getBoundingClientRect();
            const centerX = rect.left + rect.width / 2;
            const centerY = rect.top + rect.height / 2;
            const currentAngle = Math.atan2(e.clientY - centerY, e.clientX - centerX);
            const deltaAngle = (currentAngle - startAngle) * (180 / Math.PI);
            
            this.currentRotation = startRotation + deltaAngle;
            this.updateWheelRotation();
            this.updateActiveSegment();
        });
        
        document.addEventListener('mouseup', () => {
            isDragging = false;
            this.wheel.style.cursor = 'grab';
        });
        
        // Prevent context menu on right click
        this.wheel.addEventListener('contextmenu', (e) => {
            e.preventDefault();
        });
    }
    
    handleScroll(deltaY) {
        const now = Date.now();
        const timeDiff = now - this.lastScrollTime;
        
        // Determine scroll direction
        if (deltaY > 0) {
            this.scrollDirection = 'down';
        } else if (deltaY < 0) {
            this.scrollDirection = 'up';
        }
        
        // Apply rotation with speed multiplier
        const rotationAmount = (deltaY * 0.5) * this.rotationSpeed;
        this.currentRotation += rotationAmount;
        
        // Update wheel rotation
        this.updateWheelRotation();
        this.updateActiveSegment();
        
        // Update last scroll time
        this.lastScrollTime = now;
        
        // Clear scroll direction after a delay
        setTimeout(() => {
            this.scrollDirection = 'none';
            this.updateDisplay();
        }, 500);
        
        this.updateDisplay();
    }
    
    updateWheelRotation() {
        this.wheelSegments.style.transform = `rotate(${this.currentRotation}deg)`;
        this.updateDisplay();
    }
    
    updateActiveSegment() {
        // Remove active class from all segments
        document.querySelectorAll('.segment').forEach(segment => {
            segment.classList.remove('active');
        });
        
        // Calculate which segment is at the top (indicator position)
        const normalizedRotation = ((this.currentRotation % 360) + 360) % 360;
        const segmentAngle = 360 / this.segmentCount;
        const activeIndex = Math.floor(((360 - normalizedRotation) / segmentAngle) % this.segmentCount);
        
        // Add active class to current segment
        const activeSegment = document.querySelector(`.segment[data-index="${activeIndex}"]`);
        if (activeSegment) {
            activeSegment.classList.add('active');
            this.activeSegment = activeIndex + 1;
        }
        
        this.updateDisplay();
    }
    
    selectSegment(index) {
        const segmentAngle = 360 / this.segmentCount;
        const targetRotation = 360 - (index * segmentAngle);
        
        // Animate to the selected segment
        this.animateToRotation(targetRotation);
    }
    
    animateToRotation(targetRotation) {
        const startRotation = this.currentRotation;
        const duration = 1000;
        const startTime = performance.now();
        
        const animate = (currentTime) => {
            const elapsed = currentTime - startTime;
            const progress = Math.min(elapsed / duration, 1);
            
            // Easing function for smooth animation
            const easeOutQuart = 1 - Math.pow(1 - progress, 4);
            
            this.currentRotation = startRotation + (targetRotation - startRotation) * easeOutQuart;
            this.updateWheelRotation();
            this.updateActiveSegment();
            
            if (progress < 1) {
                requestAnimationFrame(animate);
            }
        };
        
        requestAnimationFrame(animate);
    }
    
    toggleSpin() {
        if (this.isSpinning) {
            this.stopSpin();
        } else {
            this.startSpin();
        }
    }
    
    startSpin() {
        this.isSpinning = true;
        this.wheelSegments.classList.add('spinning');
        this.centerCircle.textContent = '⏸️';
    }
    
    stopSpin() {
        this.isSpinning = false;
        this.wheelSegments.classList.remove('spinning');
        this.updateCenterCircle();
    }
    
    resetWheel() {
        this.currentRotation = 0;
        this.isSpinning = false;
        this.scrollDirection = 'none';
        this.activeSegment = null;
        
        this.wheelSegments.classList.remove('spinning');
        this.updateWheelRotation();
        this.updateActiveSegment();
        this.updateCenterCircle();
        
        // Reset sliders to default values
        this.segmentCountSlider.value = 8;
        this.wheelSizeSlider.value = 300;
        this.rotationSpeedSlider.value = 1;
        
        this.segmentCount = 8;
        this.wheelSize = 300;
        this.rotationSpeed = 1;
        
        this.createSegments();
        this.updateDisplay();
    }
    
    updateCenterCircle() {
        if (!this.isSpinning) {
            this.centerCircle.textContent = this.activeSegment || '•';
        }
    }
    
    updateDisplay() {
        this.currentRotationDisplay.textContent = `${Math.round(this.currentRotation)}°`;
        this.activeSegmentDisplay.textContent = this.activeSegment || 'None';
        this.scrollDirectionDisplay.textContent = this.scrollDirection;
        this.updateCenterCircle();
    }
}

// Initialize the scroll wheel when the page loads
document.addEventListener('DOMContentLoaded', () => {
    new ScrollWheel();
    
    // Add some helpful instructions
    console.log('VScore.dev Scroll Wheel PoC Loaded!');
    console.log('Controls:');
    console.log('- Mouse wheel: Rotate wheel');
    console.log('- Arrow keys: Rotate wheel');
    console.log('- Spacebar: Toggle spinning animation');
    console.log('- R key: Reset wheel');
    console.log('- Click segments: Jump to segment');
    console.log('- Drag wheel: Manual rotation');
});