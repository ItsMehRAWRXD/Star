const labels = [
  "Overview",
  "Features",
  "Docs",
  "Playground",
  "Pricing",
  "Changelog",
  "Careers",
  "Contact",
];

const wheelElement = document.getElementById("wheel");
const wrapperElement = document.getElementById("wheel-wrapper");
const selectedValueElement = document.getElementById("selected-value");

// Build segments around the circle
typeof labels !== "undefined" && labels.forEach((label, index) => {
  const segment = document.createElement("div");
  segment.className = "segment";
  segment.textContent = label;
  wheelElement.appendChild(segment);
});

const state = {
  currentAngleDeg: 0,
  targetAngleDeg: 0,
  lastWheelAt: 0,
  isPointerDown: false,
  pointerStartAngleDeg: 0,
  angleAtPointerStartDeg: 0,
};

const totalItems = labels.length;
const degPerItem = 360 / totalItems;

function normalizeAngleDeg(angle) {
  let a = angle % 360;
  if (a < 0) a += 360;
  return a;
}

function getActiveIndexFromAngle(angleDeg) {
  const normalized = normalizeAngleDeg(angleDeg);
  // 0deg is at 12 o'clock. We want item centers at multiples of degPerItem.
  const index = Math.round(normalized / degPerItem) % totalItems;
  return (totalItems - index) % totalItems; // reverse direction since we rotate wheel, not pointer
}

function snapTargetToNearestItem() {
  const normalized = normalizeAngleDeg(state.targetAngleDeg);
  const snappedSteps = Math.round(normalized / degPerItem);
  const snapped = snappedSteps * degPerItem;
  // Keep continuity to avoid big jumps (allow adding 360 multiples)
  const currentTurns = Math.round((state.targetAngleDeg - normalized) / 360);
  state.targetAngleDeg = snapped + currentTurns * 360;
}

function updateSelectedReadout() {
  const activeIndex = getActiveIndexFromAngle(state.currentAngleDeg);
  selectedValueElement.textContent = labels[activeIndex] ?? "—";
  // highlight active
  const segmentNodes = wheelElement.querySelectorAll(".segment");
  segmentNodes.forEach((node, idx) => {
    node.classList.toggle("is-active", idx === activeIndex);
  });
}

function layoutSegments() {
  const radiusPx = wrapperElement.clientWidth / 2 - 48; // padding from rim
  const segmentNodes = wheelElement.querySelectorAll(".segment");
  segmentNodes.forEach((node, idx) => {
    const theta = (idx / totalItems) * Math.PI * 2; // radians
    const x = Math.sin(theta) * radiusPx;
    const y = -Math.cos(theta) * radiusPx; // negative because screen y grows downward
    node.style.transform = `translate(-50%, -50%) translate(${x}px, ${y}px)`;
  });
}

function applyRotation() {
  wheelElement.style.transform = `rotate(${state.currentAngleDeg}deg)`;
}

// Smoothly ease currentAngle towards targetAngle
let rafId = 0;
function tick() {
  const stiffness = 0.12; // higher -> snappier
  const damping = 0.75;   // lower -> more oscillation

  const delta = state.targetAngleDeg - state.currentAngleDeg;
  const step = delta * stiffness;
  state.currentAngleDeg += step;

  // settle
  if (Math.abs(delta) < 0.05) {
    state.currentAngleDeg = state.targetAngleDeg;
  }

  applyRotation();
  updateSelectedReadout();
  rafId = requestAnimationFrame(tick);
}

function onWheel(event) {
  event.preventDefault();
  const sensitivity = 0.25; // deg per deltaY unit
  state.targetAngleDeg += event.deltaY * sensitivity;
  state.lastWheelAt = performance.now();
}

function angleFromCenter(clientX, clientY) {
  const rect = wrapperElement.getBoundingClientRect();
  const cx = rect.left + rect.width / 2;
  const cy = rect.top + rect.height / 2;
  const dx = clientX - cx;
  const dy = clientY - cy;
  const rad = Math.atan2(dy, dx); // [-PI, PI], 0 at +x
  // convert to degrees with 0 at 12 o'clock
  const deg = (rad * 180) / Math.PI + 90; // shift so 0 is at -y
  return deg;
}

function onPointerDown(event) {
  wrapperElement.setPointerCapture(event.pointerId);
  state.isPointerDown = true;
  state.pointerStartAngleDeg = angleFromCenter(event.clientX, event.clientY);
  state.angleAtPointerStartDeg = state.targetAngleDeg;
}

function onPointerMove(event) {
  if (!state.isPointerDown) return;
  const currentPointerAngle = angleFromCenter(event.clientX, event.clientY);
  let delta = currentPointerAngle - state.pointerStartAngleDeg;
  // unwrap across 180/-180 to avoid jump
  if (delta > 180) delta -= 360;
  if (delta < -180) delta += 360;
  state.targetAngleDeg = state.angleAtPointerStartDeg + delta;
}

function onPointerUp(event) {
  state.isPointerDown = false;
  snapTargetToNearestItem();
}

function onKeyDown(event) {
  if (event.key === "ArrowUp" || event.key === "ArrowLeft") {
    state.targetAngleDeg += degPerItem; // rotate wheel clockwise -> previous item
    snapTargetToNearestItem();
  } else if (event.key === "ArrowDown" || event.key === "ArrowRight") {
    state.targetAngleDeg -= degPerItem; // rotate wheel counter-clockwise -> next item
    snapTargetToNearestItem();
  }
}

// Auto-snap after a short idle following wheel scrolling
setInterval(() => {
  const now = performance.now();
  if (!state.isPointerDown && now - state.lastWheelAt > 180 && Math.abs(state.targetAngleDeg - state.currentAngleDeg) < 2) {
    snapTargetToNearestItem();
  }
}, 120);

// Init
layoutSegments();
applyRotation();
updateSelectedReadout();
rafId = requestAnimationFrame(tick);

// Events
wrapperElement.addEventListener("wheel", onWheel, { passive: false });
wrapperElement.addEventListener("pointerdown", onPointerDown);
window.addEventListener("pointermove", onPointerMove);
window.addEventListener("pointerup", onPointerUp);
window.addEventListener("resize", layoutSegments);
wrapperElement.tabIndex = 0;
wrapperElement.addEventListener("keydown", onKeyDown);