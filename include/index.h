const char ledIndex[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
    <head>
        <script>
            function changestatus() {
                btn = document.getElementById("onoff");
                if (btn.innerHTML == "STATUS: ON") {
                    var xhttp = new XMLHttpRequest();
                    xhttp.open("GET", "/ledOff", true);
                    xhttp.send();
                    btn.innerHTML = "STATUS: OFF";
                    btn.style.color = "red";
                } else if (btn.innerHTML == "STATUS: OFF") {
                    var xhttp = new XMLHttpRequest();
                    xhttp.open("GET", "/ledOn", true);
                    xhttp.send();
                    btn.innerHTML = "STATUS: ON";
                    btn.style.color = "lime";
                }
            }
            function sendColor() {
                var xhttp = new XMLHttpRequest();
                valR = document.getElementById("colorInputR").value;
                valG = document.getElementById("colorInputG").value;
                valB = document.getElementById("colorInputB").value;
                btn = document.getElementById("onoff");
                if (btn.innerHTML == "STATUS: ON") {
                    xhttp.open("GET", "/color?r="+valR+"&g="+valG+"&b="+valB, true);
                    xhttp.send();
                }
            }
        </script>
    </head>
        <body>
            <h1>LED CONTROL</h1>
            <button id="onoff" onclick="changestatus()">STATUS: OFF</button><br>
            <label>RED</label>
            <input type="number" value=0 min=0 max=255 id="colorInputR"></input><br>
            <label>GREEN</label>
            <input type="number" value=0 min=0 max=255 id="colorInputG"></input><br>
            <label>BLUE</label>
            <input type="number" value=0 min=0 max=255 id="colorInputB"></input><br>
            <button id="apply" onclick="sendColor()">SEND</button>
        </body>
</html>
)=====";