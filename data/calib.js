var contenedorLista = document.getElementById("contenerDeLista");

var estadoActual;

var flag = 0;

var verDatos = document.getElementById("vistaDatos");

var responseIngreso;

function IngresarDatos() {
    // Supongamos que deseas enviar el dato 1234 al servidor.

    // Objeto de datos que deseas enviar.
    var datos = {
        led: 1234
    };

    // Realiza la solicitud POST con AJAX.
    $.ajax({
        type: "GET", // Método de la solicitud.
        url: "/obtenerMedicion",
        data: datos, // URL del servidor al que estás enviando los datos.
        // Los datos que estás enviando.

        success: function (response) {
            // La función que se ejecutará si la solicitud es exitosa.

            responseIngreso = response.split("&");
            /****** response.split("&") es una funcion para separar en una matriz los elementos que
            esten separados por el caracter seleccionado.**********/

            console.log('respon: ' + responseIngreso);

        },
        error: function (error) {
            // La función que se ejecutará si hay un error en la solicitud.
            console.error("Error en la solicitud", error);
        }
    });
}// Recibe los datos del ESP desde el archivo /main.cpp y los guarda para separarlos.

IngresarDatos();

function agregarDatos() {
    var contenidoDeLista = document.createElement("li");

    contenidoDeLista.classList = "contenidoDeLista";

    contenidoDeLista.innerHTML = callStateValue(flag);

    verDatos.appendChild(contenidoDeLista);

    contenedorLista.scrollTop = contenedorLista.scrollHeight;
}// Pasa la informacion al archivo /index.html para muestrearlos en la lista definida de muestra de datos.

function callStateValue(ingreso) {
    console.log('el response: ' +responseIngreso)
    responseIngreso[ingreso];

    return responseIngreso[ingreso];
}// Devuelve la posicion dependiendo del estado actual de la variable.

function cambiodeFlag(numberFunction) {
    flag = numberFunction;
}// Recupera el valor dado en los botones del /index.html y lo guarda en la variable.

setInterval(() => {
    IngresarDatos();
    agregarDatos();
}, 500);// Intervalo de espera.