const char ledIndex[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
    <head>
        <style>
            * {
                background-color: black;
                font-family: Arial, Helvetica, sans-serif
            }

            p, h1, label {
                color: white;
                font-size: 40px;
            }
            button {
                background-color: black;
                color: white;
                border: solid white;
                margin: 10px;
                font-size: 40px;
                border-radius: 15px;
            }
            input {
                color: white;
                margin: 10px;
                font-size: 40px;
                border-radius: 15px;
            }
            #mainContainer {
                border: solid white;
                border-width: 10px;
                text-align: center;
                margin: 0 auto;
                padding: 10px;
            }
            @media (pointer:none), (pointer:coarse) {
                p, h1, label {
                    font-size: 70px;
                }
                button, input {
                    font-size: 70px;
                }
            }
        </style>
    </head>
        <body>
            <section id="mainContainer">
            <h1>LED CONTROL</h1>
            %USERINPUT%
            </section>
            <script>
            function changestatus() {
                var btn = document.getElementById("onoff");
                var xhttp = new XMLHttpRequest();
                xhttp.onreadystatechange = function() {
                    if (this.readyState == 4 && this.status == 200) {
                        btn.innerHTML = this.responseText;
                    }
                };
                xhttp.open("GET", "/ledToggle", true);
                xhttp.send();
            }
            function sendColor() {
                var xhttp = new XMLHttpRequest();
                var valR = document.getElementById("colorInputR").value;
                var valG = document.getElementById("colorInputG").value;
                var valB = document.getElementById("colorInputB").value;
                var btn = document.getElementById("onoff");
                xhttp.onreadystatechange = function() {
                    if (this.readyState == 4 && this.status == 200) {
                        const obj = JSON.parse(this.responseText);
                        valR = Number(obj.r);
                        valG = Number(obj.g);
                        valB = Number(obj.b);
                    }
                };
                xhttp.open("GET", "/color?r="+valR+"&g="+valG+"&b="+valB, true);
                xhttp.send();
            }
            function updateColor() {
                var valR = toHex(document.getElementById("colorInputR").value);
                var valG = toHex(document.getElementById("colorInputG").value);
                var valB = toHex(document.getElementById("colorInputB").value);
                string = ("#" + valR+valG+valB).toString();
                document.getElementById("mainContainer").style.borderColor = string;
            }
            function toHex(d) {
                return  ("0"+(Number(d).toString(16))).slice(-2).toUpperCase()
            }
            updateColor();
            </script>
        </body>
</html>
)=====";
const char updateIndex[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
    <head>
        <style>
            * {
                background-color: black;
                font-family: Arial, Helvetica, sans-serif
            }
            p, h1, label {
                color: white;
                font-size: 40px;
            }
            button {
                background-color: black;
                color: white;
                border: solid white;
                margin: 10px;
                font-size: 40px;
                border-radius: 15px;
            }
            input {
                color: white;
                margin: 10px;
                font-size: 20px;
                border-radius: 15px;
            }
            #mainContainer {
                border: solid white;
                border-width: 10px;
                text-align: center;
                margin: 0 auto;
                padding: 10px;
            }
            @media (pointer:none), (pointer:coarse) {
                p, h1, label {
                    font-size: 70px;
                }
                button, input {
                    font-size: 70px;
                }
            }
        </style>
        <script>
            function notify_update() {
                document.getElementById("update").innerHTML = "<p>Updating...</p>"; 
            }
        </script>
    </head>
    <body>
        <section id="mainContainer">
            <h1>OTA Updater</h1>
            <p>Only *.bin files will work!</p>
            <form method='POST' action='/doUpdate' enctype='multipart/form-data' target='_self' onsubmit='notify_update()'>
                <input type='file' name='update' accept=".bin"><br>
                <input type='submit' value='Start Update!'></form>
            <div id="update"></div>
        </section>
    </body>
</html>
)=====";
const char updateDoneIndex[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
    <head>
        <style>
            * {
                background-color: black;
                font-family: Arial, Helvetica, sans-serif
            }
            p, h1, label {
                color: white;
                font-size: 40px;
            }
            @media (pointer:none), (pointer:coarse) {
                p, h1, label {
                    font-size: 70px;
                }
            }
        </style>
        <script>
            setTimeout(refresh, 5000);
            function refresh() {
                window.location.replace("/");
            }
        </script>
    </head>
    <body>
        <p>Update done! Refreshing in 5 seconds...</p>
    </body>
</html>
)=====";