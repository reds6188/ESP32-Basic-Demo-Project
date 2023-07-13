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
    });
    return response.json();
}

function polling(delay) {
    setTimeout(() => {
        console.log('request data...')
        getData(`/status`).then((data) => {
            showLoadingPage(false);
            console.log(data);
            const date = new Date(data.timestamp);
            document.getElementById("timestamp").innerText = date.toLocaleString('it-IT');
            document.getElementById("temperature").innerText = `${data.temperature.toFixed(1)}°C`
            document.getElementById("pressure").innerText = `${data.pressure.toFixed(1)} hPa`
            document.getElementById("humidity").innerText = `${data.humidity.toFixed(1)} %`
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
    getData(`/version`).then((data) => {
        document.getElementById("footer").innerText = `${data.version} (${data.date})`;
        polling(500);
    }).catch((err) => {
        console.error(err);
        polling(500);
    });
}

