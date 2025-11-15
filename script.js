// --- Global State ---
let cities = []; 
let roads = []; 
let selectedCityName = null; // Tracks the first city selected for a road in Build mode
let routeStartName = null; // Tracks the starting city for routing
let routeEndName = null;   // Tracks the ending city for routing
let currentMode = 'none';  // 'none', 'addCity', 'addRoad'

// --- DOM Elements ---
const output = document.getElementById('output');
const canvas = document.getElementById('graph-canvas');
const ctx = canvas.getContext('2d');

// --- Config ---
const NODE_RADIUS = 15;
const NODE_COLOR = '#3b82f6';
const ROUTE_START_COLOR = '#10b981';
const ROUTE_END_COLOR = '#ef4444';
const ROAD_COLOR = '#6b7280';
const PATH_COLOR = '#2563eb';

// --- I. CORE DATA STRUCTURES AND ALGORITHMS ---

class PriorityQueue {
    constructor() { this.values = []; }
    enqueue(val, priority) {
        this.values.push({ val, priority });
        // Simple sort for easier maintenance/demo instead of a full min-heap implementation
        this.values.sort((a, b) => a.priority - b.priority); 
    }
    dequeue() { return this.values.shift(); }
    isEmpty() { return this.values.length === 0; }
}

/** Builds the Adjacency List from the global state. */
function buildAdjacencyList() {
    const adj = new Map();
    cities.forEach(city => adj.set(city.name, []));
    roads.forEach(road => {
        // NOTE: Road storage is now bi-directional, but the adjacency list must still be built
        // to handle the connections regardless of which city was clicked first during creation.
        adj.get(road.from).push({ node: road.to, weight: road.distance });
    });
    return adj;
}

/** Runs Dijkstra's Algorithm for shortest weighted path. */
function runDijkstraLogic(startName, endName) {
    const adj = buildAdjacencyList();
    const distances = {};
    const previous = {};
    const pq = new PriorityQueue();

    cities.forEach(city => {
        distances[city.name] = city.name === startName ? 0 : Infinity;
        previous[city.name] = null;
        pq.enqueue(city.name, distances[city.name]);
    });

    while (!pq.isEmpty()) {
        const current = pq.dequeue().val;
        if (current === endName) break;
        if (distances[current] === Infinity) continue;

        // Iterate over neighbors from the Adjacency List
        for (const neighbor of adj.get(current)) {
            const newDist = distances[current] + neighbor.weight;
            if (newDist < distances[neighbor.node]) {
                distances[neighbor.node] = newDist;
                previous[neighbor.node] = current;
                pq.enqueue(neighbor.node, newDist);
            }
        }
    }

    if (distances[endName] === Infinity) return null;

    let path = [endName];
    let currentCity = endName;
    while (currentCity !== startName) {
        currentCity = previous[currentCity];
        path.unshift(currentCity);
    }
    return { path, distance: distances[endName] };
}

/** Runs BFS for shortest unweighted path (fewest stops). */
function runBFSLogic(startName, endName) {
    const adj = buildAdjacencyList();
    const queue = [startName];
    const visited = new Set([startName]);
    const previous = {};

    let found = false;
    while (queue.length > 0) {
        const current = queue.shift();
        if (current === endName) { found = true; break; }
        for (const neighbor of adj.get(current)) {
            if (!visited.has(neighbor.node)) {
                visited.add(neighbor.node);
                previous[neighbor.node] = current;
                queue.push(neighbor.node);
            }
        }
    }

    if (!found) return null;
    let path = [endName];
    let current = endName;
    while (current !== startName) {
        current = previous[current];
        path.unshift(current);
    }
    return { path };
}

// --- II. VISUALIZATION AND DRAWING ---

/** Main drawing function for the Canvas. */
function drawGraph(path = []) {
    if (!ctx) return;
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    const pathRoads = new Set();
    // Build set of roads in the path for highlighting
    for (let i = 0; i < path.length - 1; i++) {
        // Add both directions to highlight correctly, as roads are bi-directional now
        pathRoads.add(`${path[i]}->${path[i + 1]}`);
        pathRoads.add(`${path[i + 1]}->${path[i]}`);
    }

    // 1. Draw Roads (Ensure we only draw each physical road once)
    const drawnRoads = new Set();
    roads.forEach(road => {
        // Canonical key to ensure a road is only drawn once (e.g., A-B is the same as B-A)
        const key = [road.from, road.to].sort().join('-');
        
        if (drawnRoads.has(key)) return;
        drawnRoads.add(key);

        const fromCity = cities.find(c => c.name === road.from);
        const toCity = cities.find(c => c.name === road.to);
        if (!fromCity || !toCity) return;

        // Check if either direction of the road is part of the path
        const isPathRoad = pathRoads.has(`${road.from}->${road.to}`) || pathRoads.has(`${road.to}->${road.from}`);
        
        ctx.beginPath();
        ctx.moveTo(fromCity.x, fromCity.y);
        ctx.lineTo(toCity.x, toCity.y);
        
        ctx.strokeStyle = isPathRoad ? PATH_COLOR : ROAD_COLOR;
        ctx.lineWidth = isPathRoad ? 4 : 2;
        ctx.stroke();

        // Draw distance label
        const midX = (fromCity.x + toCity.x) / 2;
        const midY = (fromCity.y + toCity.y) / 2;
        
        // Draw background box for text
        ctx.fillStyle = isPathRoad ? PATH_COLOR : ROAD_COLOR;
        ctx.fillRect(midX - 15, midY - 10, 30, 20); 
        
        // Draw text
        ctx.fillStyle = 'white';
        ctx.font = '600 10px Inter';
        ctx.textAlign = 'center';
        ctx.fillText(road.distance, midX, midY + 4);
    });

    // 2. Draw Nodes
    cities.forEach(city => {
        const color = city.name === routeStartName ? ROUTE_START_COLOR : 
                      city.name === routeEndName ? ROUTE_END_COLOR : 
                      NODE_COLOR;

        // Draw selection glow
        ctx.shadowColor = color;
        // Increased glow for selected items (selectedCityName is for road building)
        ctx.shadowBlur = city.name === selectedCityName || city.name === routeStartName || city.name === routeEndName ? 10 : 0;
        
        // Main circle
        ctx.beginPath();
        ctx.arc(city.x, city.y, NODE_RADIUS, 0, Math.PI * 2);
        ctx.fillStyle = color;
        ctx.fill();

        // Draw white center dot (aesthetic)
        ctx.shadowBlur = 0;
        ctx.fillStyle = 'white';
        ctx.beginPath();
        ctx.arc(city.x, city.y, 4, 0, Math.PI * 2);
        ctx.fill();

        // City name
        ctx.fillStyle = color;
        ctx.font = '600 12px Inter';
        ctx.textAlign = 'center';
        ctx.fillText(city.name, city.x, city.y + NODE_RADIUS + 18);
    });
}

// --- III. UI AND INTERACTION HELPERS ---

/** Logs a message to the output console with a timestamp. */
function log(msg) {
    const timestamp = new Date().toLocaleTimeString('en-US', { hour12: false });
    output.textContent = `[${timestamp}] > ${msg}\n${output.textContent}`;
}

/** Sets the current interaction mode and updates the UI. */
function setMode(mode) {
    // Check if toggling the same mode off
    if (currentMode === mode && mode !== 'none') {
        currentMode = 'none'; 
        log(`Mode **${mode}** deactivated.`);
    } else {
        currentMode = mode;
        if (mode !== 'none') {
            log(`Mode **${mode}** activated. Click on the canvas or cities to proceed.`);
        } else {
            log(`Selection cleared. Mode is **None**.`);
        }
    }

    // Update button visual state
    document.querySelectorAll('.btn-action').forEach(btn => btn.classList.remove('active-mode'));
    if (currentMode !== 'none') {
        const btnElement = document.getElementById(`${currentMode}-btn`);
        if (btnElement) {
            btnElement.classList.add('active-mode');
        }
    }

    // Reset temporary selection when changing/toggling mode
    selectedCityName = null;
    drawGraph();
}

/** Updates stats panels and route dropdowns. */
function updateUI() {
    // Update stats
    document.getElementById('city-count').textContent = cities.length;
    
    // Road count needs to be updated because we store 2 entries for 1 physical road
    // We can count the roads array, but it's more accurate to count unique physical roads
    const uniqueRoads = new Set();
    roads.forEach(r => uniqueRoads.add([r.from, r.to].sort().join('-')));
    const roadCount = uniqueRoads.size;

    document.getElementById('road-count').textContent = roadCount;
    document.getElementById('viz-city-count').textContent = cities.length;
    document.getElementById('viz-road-count').textContent = roadCount;

    // Update route selection dropdowns
    const cityNames = cities.map(c => c.name);
    const optionsHtml = cityNames.map(name => `<option value="${name}">${name}</option>`).join('');
    
    // Preserve state
    const currentStart = document.getElementById('source-city').value;
    const currentEnd = document.getElementById('destination-city').value;

    document.getElementById('source-city').innerHTML = `<option value="">Select Source</option>` + optionsHtml;
    document.getElementById('destination-city').innerHTML = `<option value="">Select Destination</option>` + optionsHtml;

    document.getElementById('source-city').value = currentStart;
    document.getElementById('destination-city').value = currentEnd;

    drawGraph();
}

/** Switches the active tab and resets transient states. */
function showTab(tabId) {
    document.querySelectorAll('.tab-content').forEach(tab => tab.classList.add('hidden'));
    document.querySelectorAll('.tab-btn').forEach(btn => btn.classList.remove('active'));

    document.getElementById(`${tabId}-tab`).classList.remove('hidden');
    document.querySelector(`.tab-btn[data-tab="${tabId}"]`).classList.add('active');
    
    // Reset selections and mode on tab switch
    selectedCityName = null;
    routeStartName = null;
    routeEndName = null;
    currentMode = 'none'; 
    updateUI();
}

/** Syncs dropdown selection with canvas state. */
function updateRouteSelection(type, value) {
    if (type === 'start') {
        routeStartName = value || null;
    } else if (type === 'end') {
        routeEndName = value || null;
    }
    drawGraph();
}

// --- IV. CANVAS EVENT HANDLERS ---

canvas.addEventListener('click', (event) => {
    // Get accurate mouse position relative to the canvas
    const rect = canvas.getBoundingClientRect();
    const pos = { x: event.clientX - rect.left, y: event.clientY - rect.top };
    
    // Find if an existing city was clicked
    const clickedCity = cities.find(city => {
        const dx = city.x - pos.x;
        const dy = city.y - pos.y;
        return Math.sqrt(dx * dx + dy * dy) < NODE_RADIUS;
    });

    handleCanvasClick(pos, clickedCity);
});

/** Main handler for canvas click events based on active mode. */
function handleCanvasClick(pos, clickedCity) {
    const activeTab = document.querySelector('.tab-btn.active').getAttribute('data-tab');

    if (activeTab === 'build') {
        if (currentMode === 'addCity' && !clickedCity) {
            handleAddNewCity(pos);
        } else if (currentMode === 'addRoad' && clickedCity) {
            handleRoadSelection(clickedCity);
        } else if (currentMode === 'none') {
             log(`Mode is **None**. Click 'Add City' or 'Add Road' first.`);
        } else if (!clickedCity && currentMode === 'addRoad') {
            log(`To add a road, click two existing cities.`);
        } else if (clickedCity && currentMode === 'addCity') {
             log(`Cannot add a city on top of **${clickedCity.name}**. Click an empty area.`);
        }
    } else if (activeTab === 'find') {
        handleFindRouteClick(clickedCity);
    }
    drawGraph();
}

/** Logic for adding a new city (runs only in 'addCity' mode on empty space). */
function handleAddNewCity(pos) {
    const newCityName = prompt("Enter the name for the new city:");
    if (newCityName && newCityName.trim()) {
        const trimmedName = newCityName.trim();
        if (!cities.some(c => c.name === trimmedName)) {
            cities.push({ 
                name: trimmedName, 
                x: pos.x, 
                y: pos.y, 
                color: NODE_COLOR 
            });
            log(`City **${trimmedName}** added.`);
            updateUI(); 
            setMode('none'); // Exit mode after adding one city
        } else {
            log("Error: City name already exists.");
        }
    } else if (newCityName !== null) { 
        log("City creation cancelled or name was empty.");
    }
}

/** Logic for selecting cities to form a road (runs only in 'addRoad' mode on existing cities). */
function handleRoadSelection(clickedCity) {
    if (!selectedCityName) {
        selectedCityName = clickedCity.name;
        log(`Selected **${selectedCityName}**. Click the second city to define a road.`);
    } else if (selectedCityName === clickedCity.name) {
        selectedCityName = null; // Deselect
        log(`Deselected **${clickedCity.name}**.`);
    } else {
        // Road creation is ready
        showAddRoadModal(selectedCityName, clickedCity.name);
        // The mode is cleared in confirmAddRoad after success
    }
}

function handleFindRouteClick(clickedCity) {
    if (!clickedCity) return;
    
    // Logic for sequential clicking to set start and end points
    if (!routeStartName) {
        routeStartName = clickedCity.name;
        routeEndName = null;
    } else if (routeStartName === clickedCity.name) {
        routeStartName = null;
    } else if (!routeEndName) {
        routeEndName = clickedCity.name;
    } else if (routeEndName === clickedCity.name) {
        routeEndName = null;
    } else {
        // Start a new sequence
        routeStartName = clickedCity.name;
        routeEndName = null;
    }
    
    // Sync dropdowns
    document.getElementById('source-city').value = routeStartName || '';
    document.getElementById('destination-city').value = routeEndName || '';
    
    drawGraph();
    if (routeStartName && routeEndName) {
        log(`Route selected: **${routeStartName}** to **${routeEndName}**. Click Compute.`);
    } else if (routeStartName) {
        log(`Source set to **${routeStartName}**. Select destination.`);
    }
}


// --- V. MODAL AND COMPUTATION ---

/** Shows the modal to get distance input for a new road. */
function showAddRoadModal(from, to) {
    document.getElementById('modal-body').innerHTML = `
        <h3>Add Road</h3>
        <p>Road from <b>${from}</b> to <b>${to}</b></p>
        <label for="roadDist">Distance (km):</label>
        <input type="number" id="roadDist" placeholder="Enter distance" min="1">
        <button onclick="confirmAddRoad('${from}', '${to}')">Add Road</button>
    `;
    document.getElementById('modal').classList.remove('hidden');
}

/** Confirms road creation after distance input. */
function confirmAddRoad(from, to) {
    const dist = parseInt(document.getElementById('roadDist').value);
    
    if (isNaN(dist) || dist <= 0) {
        alert("Distance must be a positive number."); 
        return;
    }
    
    // Check if the physical road (A-B or B-A) already exists
    const roadExists = roads.some(r => 
        (r.from === from && r.to === to) || 
        (r.from === to && r.to === from)
    );
    if (roadExists) {
        alert("Road already exists between these two cities.");
        return;
    }

    // FIX: Add two entries to the 'roads' array for bi-directional travel
    roads.push({ from: from, to: to, distance: dist }); // A -> B
    roads.push({ from: to, to: from, distance: dist }); // B -> A (The Fix!)
    
    log(`Bi-directional road added: ${from} <-> ${to} (${dist} km)`);
    closeModal();
    selectedCityName = null;
    setMode('none'); // Exit mode after successful road creation
    updateUI();
}

/** Closes the modal. */
function closeModal() {
    document.getElementById('modal').classList.add('hidden');
    // If the user cancels road creation, clear selection
    if (currentMode === 'addRoad') {
        selectedCityName = null;
        drawGraph();
    }
}

/** Executes the selected pathfinding algorithm. */
function computeRoute() {
    const start = document.getElementById('source-city').value;
    const end = document.getElementById('destination-city').value;
    const algorithm = document.getElementById('algorithm-select').value;
    
    if (!start || !end) {
        log("Error: Please select both a source and a destination city.");
        return;
    }
    if (start === end) {
        log(`Path is 0 km: Already at ${start}.`);
        drawGraph();
        return;
    }

    let result = null;
    if (algorithm === 'dijkstra') {
        result = runDijkstraLogic(start, end);
        if (result) {
            log(`**Dijkstra's Result**:\nPath: ${result.path.join(' -> ')}\nDistance: ${result.distance} km`);
        }
    } else if (algorithm === 'bfs') {
        result = runBFSLogic(start, end);
        if (result) {
            log(`**BFS Result**:\nPath: ${result.path.join(' -> ')}\nStops: ${result.path.length - 1}`);
        }
    }

    if (!result) {
        log(`**NO PATH FOUND** from ${start} to ${end}.`);
        drawGraph();
    } else {
        drawGraph(result.path);
    }
}

// Debug functions for console output
function displayGraphData() {
    const adj = buildAdjacencyList();
    let result = "===== ADJACENCY LIST (BI-DIRECTIONAL) =====\n";
    cities.forEach(city => {
        const connections = adj.get(city.name).map(edge => `${edge.node} (${edge.weight}km)`);
        result += `${city.name}: ${connections.join(' | ') || '(No outgoing roads)'}\n`;
    });
    log(result);
}

function displayMatrixData() {
    let matrix = "===== ADJACENCY MATRIX (0 = No Road) =====\n\n      ";
    const padding = 12;

    cities.forEach(c => matrix += c.name.padEnd(padding));
    matrix += "\n";

    cities.forEach(i => {
        matrix += i.name.padEnd(6);
        cities.forEach(j => {
            // Find the road from i to j (only one direction is needed for the matrix value)
            const road = roads.find(r => r.from === i.name && r.to === j.name);
            const value = road ? road.distance.toString() : '0'; 
            matrix += value.padEnd(padding);
        });
        matrix += "\n";
    });

    log(matrix);
}

// --- INITIALIZATION ---
window.onload = () => {
    // Initial canvas setup
    const resizeCanvas = () => {
        canvas.width = canvas.offsetWidth;
        canvas.height = canvas.offsetHeight;
        drawGraph();
    };
    window.addEventListener('resize', resizeCanvas);
    resizeCanvas();

    updateUI();
    log("System Initialized. Click 'Add City' to begin building your graph.");
};