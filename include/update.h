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
                font-size: 30px;
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
                document.getElementById("update").innerHTML = "<h2>Updating...</h2>"; 
            } 
        </script>
    </head>
    <body>
        <section id="mainContainer">
            <h1>OTA Updater</h1>
            <p>Only *.bin files will work!</p>
            <form method='POST' action='/doUpdate' enctype='multipart/form-data' target='_self' onsubmit='notify_update()'>
                <input type='file' name='update' accept=".bin"><br>
                <input type='submit' value='Do update'></form>
            <div id="update"></div>
        </section>
    </body>
</html>
)=====";