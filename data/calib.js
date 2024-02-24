function IngresarDatos() {
    // Supongamos que deseas enviar el dato 1234 al servidor

    // Objeto de datos que deseas enviar
    var datos = {
        led: 1234
    };

    // Realiza la solicitud POST con AJAX
    $.ajax({
        type: "GET", // Método de la solicitud
        url: "/obtenerMedicion",
        data:datos, // URL del servidor al que estás enviando los datos
         // Los datos que estás enviando
        success: function (response) {
            // La función que se ejecutará si la solicitud es exitosa
            parrafo.innerHTML = response;
            responseIngreso = response.split("&");
        },
        error: function (error) {
            // La función que se ejecutará si hay un error en la solicitud
            console.error("Error en la solicitud", error);
        }
    });

}

function agregarDatos(){
    var contenidoDeLista = document.createElement("li");

    contenidoDeLista.classList = "contenidoDeLista";

    contenidoDeLista.innerHTML = cambiodeEstado;

    verDatos.appendChild(contenidoDeLista);

    contenedorLista.scrollTop = contenedorLista.scrollHeight;

}

function cambioEstado(Ingreso) {
    estadoActual = Ingreso?responseIngreso[0]: responseIngreso[1];
    return estadoActual;

}

function cambiodeFlag(){
    flag = !flag;
}

botonVolt.addEventListener("click", {flag: false});
botonRes.addEventListener("click", {flag: true});

setInterval(() => {
    cambioEstado(estadoActual);
    IngresarDatos();
    agregarDatos();
}, 500);

var contenedorLista = document.getElementById("contenerDeLista");

var estadoActual;

var flag = true;

var botonVolt = document.getElementById("botonVoltaje");

var botonRes = document.getElementById("botonResistencia");

var parrafo = document.getElementById("parrafo");

var verDatos = document.getElementById("vistaDatos");