//const HOST_NAME = `http://127.0.0.1:3000`;
const HOST_NAME = ``;
const STATUS = `${HOST_NAME}/status`;
const VERSION = `${HOST_NAME}/version`;

const units = {
    temperature: '°C',
    humidity: '%',
    pressure: 'hPa'
};


function showLoadingPage (show) {
    let page = document.getElementById('page');
    let loading = document.getElementById('loading');

    if(show === true) {
        // Hide the content and show loading page
        page.classList.replace('flex', 'hidden');
        loading.classList.replace('hidden', 'flex');
    }
    else {
        // Hide loading page and show the content
        loading.classList.replace('flex', 'hidden');
        page.classList.replace('hidden', 'flex');
    }
}

async function getData(url = "") {
    const response = await fetch(url, {
        method: "GET",
        //mode: "cors",
    });
    console.log(response.body);
    return response.body == null ? {} : response.json();
}

function polling(delay) {
    setTimeout(() => {
        console.log('request data...')
        getData(STATUS)/*.then((response) => {
            if (!response.ok) {
                throw new Error(`HTTP error: ${response.status}`);
            }
            return response.json();
        })*/.then((data) => {
            showLoadingPage(false);
            console.log(data);
            const date = new Date(data.timestamp);
            document.getElementById("timestamp").innerText = date.toLocaleString('it-IT');
            for(element of document.querySelectorAll('.data')) {
                const id = element.id
                element.querySelector('.value-curr').innerText = `${data[id].current} ${units[id]}`;
                element.querySelector('.value-min').innerText = `${data[id].min} ${units[id]}`;
                element.querySelector('.value-max').innerText = `${data[id].max} ${units[id]}`;
            }
            setTimeout(polling, delay, delay);
        }).catch((err) => {
            console.error(err);
            showLoadingPage(true);
            setTimeout(polling, delay, delay);
        });
    }, delay)
}

//polling(500);

window.addEventListener('load', onLoad);

function onLoad(event) {
    getData(VERSION).then((data) => {
        document.getElementById("footer").innerText = `${data.version} (${data.date})`;
        polling(500);
    }).catch((err) => {
        console.error(err);
        polling(500);
    });
}

