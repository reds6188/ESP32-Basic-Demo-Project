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
            console.log(data);
            setTimeout(polling, delay, delay);
        }).catch((err) => {
            console.error(err);
            setTimeout(polling, delay, delay);
        });
    }, delay)
}

polling(500);
