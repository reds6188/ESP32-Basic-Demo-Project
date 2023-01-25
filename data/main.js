let store = {
    whirlpool: {
        status: false,
    },
    blower: {
        status: false,
        value: 6,
        step: 1,
        min: 1,
        max: 10
    },
    light: {
        value: 1,
        step: 1,
        min: 1,
        max: 10
    },
    temperature: {
        value: 15,
        step: 1,
        min: 10,
        max: 40
    },
    color: {
        fixed: [
            'white', 'red', 'orange', 'yellow', 'lime', 'cyan', 'blue', 'magenta'
        ],
        cycles: [
            'energy', 'relax', 'tonic'
        ]
    }
}

let gateway = `ws://${window.location.hostname}/ws`;
//let gateway = `ws://10.16.123.107/ws`;
let websocket;

window.addEventListener('load', onLoad);

function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
}

function onOpen(event) {
    console.log('Connection opened');
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    console.log(`Process: ${event.data}`)
    let obj = JSON.parse(event.data);
    const action = obj.action;
    const func = obj.function;
    const value = obj.value;

    switch(action) {
        case 'on-off':
            let btn = document.querySelector(`#${func} > button`)
            btn.classList.add(value == 'true' ? 'grn' : 'drk-blu');
            btn.innerText = value == 'true' ? 'ON' : 'OFF';
            break;
        case 'read-chip':
            break;
        default:
            console.log('Unrecognized command:', command);
            break;

    }
}

function onLoad(event) {
    initWebSocket();
    initControls();
}

function initControls() {
    document.querySelector('#blower-speed > input').value = store.blower.value;
    document.querySelector('#temp-regulation > input').value = store.temperature.value;

    let color_selected = document.getElementById('color-selected').style.backgroundColor = 'white'
    let color_group = document.getElementById('color-group')
    for(color of store.color.fixed) {
        let box = document.createElement('div');
        box.classList.add('color-box');
        box.style.backgroundColor = color;
        box.addEventListener('click', (e) => { setColor(e); });
        color_group.appendChild(box);
    }
}

function showSection(id, tab) {
    //let id = event.target.id;

    for(element of document.getElementsByClassName('bar-icon')) {
        element.classList.remove('selected')
    }
    tab.classList.add('selected');

	const sections = document.querySelectorAll("div.section");
	for(item of sections) {
	  item.classList.add("hidden");
	}

    document.getElementById(id).classList.remove("hidden");
}

function onoffFunc(element) {
    const id = element.parentNode.id;
    console.log(id);
    element.classList.remove('drk-blu', 'grn');
    element.innerHTML = '<img src="svg/spinner.svg" class="spinner">'

    store[id].status = !store[id].status


    websocket.send(JSON.stringify({
        action: "on-off",
        function: id,
        value: store[id].status
    }));
}

function setParam(element, type) {
    const id = element.parentNode.id;
    let obj, func;
    switch(element.parentNode.id) {
        case 'blower-speed':
            obj = store.blower;
            func = 'blower';
            break;
        case 'temp-regulation':
            obj = store.temperature;
            func = 'whirlpool';
            break;
        default:
            break;
    }

    if(type === 'plus') {
        if(obj.value < obj.max)
            obj.value += obj.step;
    }
    else if(type === 'minus') {
        if(obj.value > obj.min)
            obj.value -= obj.step;
    }
    console.log(id);

    document.querySelector(`#${id} > input`).value = obj.value;

    websocket.send(JSON.stringify({
        action: "set-param",
        function: func,
        value: obj.value
    }));

}

function setColor(event) {
    let color = event.target.style.backgroundColor;
    let color_selected = document.getElementById('color-selected');
    color_selected.style.backgroundColor = color;
    color_selected.classList.remove('hidden');
    document.getElementById('color-group').style.display = 'none';
    document.removeEventListener('click', exitColorSelection, false);

    websocket.send(JSON.stringify({
        action: "set-param",
        function: "cromo",
        value: color
    }));
}

function selectColor(element) {
    element.classList.add('hidden');
    document.getElementById('color-group').style.display = 'flex';
    setTimeout(() => document.addEventListener('click', exitColorSelection, false), 100);
}

function exitColorSelection(event) {
    /*
    if(event.target != document.getElementById('color-group')) {
        document.getElementById('color-selected').classList.remove('hidden');
        document.getElementById('color-group').style.display = 'hidden';
    }
    */
    for(element of document.querySelectorAll('.color-box')) {
        inside = element.contains(event.target);
        if(inside === true)
            break;
    }

    if(inside === false) {
        document.getElementById('color-selected').classList.remove('hidden');
        document.getElementById('color-group').style.display = 'none';
        document.removeEventListener('click', exitColorSelection, false);
    }
}