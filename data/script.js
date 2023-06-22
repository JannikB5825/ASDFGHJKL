const labels = [
    "10min",
    "9min",
    "8min",
    "7min",
    "6min",
    "5min",
    "4min",
    "2min",
    "2min",
    "1min",
];
const temperatureData = {
    label: "Temperature",
    data: [],
    borderColor: "#f5c071",
    backgroundColor: "#f5c071be",
};
const humidityData = {
    label: "Humidity",
    data: [],
    borderColor: "#71e1f5",
    backgroundColor: "#71e1f5be",
};
const airpressureData = {
    label: "Air-Pressure",
    data: [],
    borderColor: "#71f578",
    backgroundColor: "#71f578be",
};

const dataTemp = {
    labels: labels,
    datasets: [temperatureData],
};
const dataHum = {
    labels: labels,
    datasets: [humidityData],
};
const dataPres = {
    labels: labels,
    datasets: [airpressureData],
};

const configTemp = {
    type: "line",
    data: dataTemp,
    options: {
        maintainAspectRatio: false,
        responsive: true,
        scales: {
            y: {
                ticks: {
                    font: {
                        size: 32,
                    },
                },
            },
            x: {
                ticks: {
                    font: {
                        size: 32,
                    },
                },
            },
        },
        elements: {
            line: {
                tension: 0.3,
            },
        },
        plugins: {
            legend: {
                labels: {
                    font: {
                        size: 32,
                    },
                },
            },
        },
    },
};

const configHum = {
    type: "line",
    data: dataHum,
    options: {
        maintainAspectRatio: false,
        responsive: true,
        scales: {
            y: {
                ticks: {
                    font: {
                        size: 32,
                    },
                },
            },
            x: {
                ticks: {
                    font: {
                        size: 32,
                    },
                },
            },
        },
        elements: {
            line: {
                tension: 0.3,
            },
        },
        plugins: {
            legend: {
                labels: {
                    font: {
                        size: 32,
                    },
                },
            },
        },
    },
};

const configPres = {
    type: "line",
    data: dataPres,
    options: {
        maintainAspectRatio: false,
        responsive: true,
        scales: {
            y: {
                ticks: {
                    font: {
                        size: 32,
                    },
                },
            },
            x: {
                ticks: {
                    font: {
                        size: 32,
                    },
                },
            },
        },
        elements: {
            line: {
                tension: 0.3,
            },
        },
        plugins: {
            legend: {
                labels: {
                    font: {
                        size: 32,
                    },
                },
            },
        },
    },
};

var temperatureChart = new Chart(
    document.getElementById("temperatureChart"),
    configTemp
);

var humidityChart = new Chart(
    document.getElementById("humidityChart"),
    configHum
);

var airpressureChart = new Chart(
    document.getElementById("airpressureChart"),
    configPres
);

var initTemp = 10;
var initHum = 10;
var initPres = 10;

var tempValue = document.getElementById("temperature-value");
var humValue = document.getElementById("humidity-value");
var presValue = document.getElementById("airpressure-value");

if (!!window.EventSource) {
    var source = new EventSource("/events");

    source.addEventListener(
        "open",
        function (e) {
            console.log("Events Connected");
        },
        false
    );
    source.addEventListener(
        "error",
        function (e) {
            if (e.target.readyState != EventSource.OPEN) {
                console.log("Events Disconnected");
            }
        },
        false
    );

    source.addEventListener(
        "message",
        function (e) {
            console.log("message", e.data);
        },
        false
    );

    source.addEventListener(
        "temperature",
        function (e) {
            console.log("temperature", e.data);
            if (initTemp > 0) {
                initTemp -= 1;
            } else {
                temperatureData.data.shift();
            }
            temperatureData.data.push(e.data);
            temperatureChart.update();

            tempValue.innerHTML = e.data;
        },
        false
    );

    source.addEventListener(
        "humidity",
        function (e) {
            console.log("humidity", e.data);
            if (initHum > 0) {
                initHum -= 1;
            } else {
                humidityData.data.shift();
            }
            humidityData.data.push(e.data);
            humidityChart.update();

            humValue.innerHTML = e.data;
        },
        false
    );
    source.addEventListener(
        "airpressure",
        function (e) {
            console.log("airpressure", e.data);
            if (initPres > 0) {
                initPres -= 1;
            } else {
                airpressureData.data.shift();
            }
            airpressureData.data.push(e.data);
            airpressureChart.update();

            presValue.innerHTML = e.data;
        },
        false
    );
}

var home = document.getElementById("home");
var histori = document.getElementById("history");
var settings = document.getElementById("settings");

function gtHome() {
    home.style.visibility = "visible";
    histori.style.visibility = "hidden";
    settings.style.visibility = "hidden";
}

function gtHistory() {
    home.style.visibility = "hidden";
    histori.style.visibility = "visible";
    settings.style.visibility = "hidden";
}

function gtSettings() {
    home.style.visibility = "hidden";
    histori.style.visibility = "hidden";
    settings.style.visibility = "visible";
}

gtHome();

async function setWlanSettings(){
    const ssid = document.getElementById("ssid").value;
    const pass = document.getElementById("password").value;
    const status = document.getElementById("statusMessage");
    const response = await fetch('/',{
        method: 'POST',
        headers: {
            'Accept': 'application/json',
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({ "ssid": ssid, "password": pass })
    });
    if(response.status == 200){
        status.innerHTML = "Settings set";
    }
    else{
        status.innerHTML = "Something went wrong";
    }


}