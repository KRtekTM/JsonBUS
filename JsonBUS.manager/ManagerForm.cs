using JsonBUS.manager.Helpers;
using JsonBUS.manager.TelemetryModules;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO.Ports;
using System.Threading;
using System.Windows.Forms;
using NickBuhro.Translit;
using System.Net.NetworkInformation;
using Newtonsoft.Json;

namespace JsonBUS.manager
{
    public partial class ManagerForm : Form
    {
        private Config _Config;
        private static List<SerialPort> Ports;
        private SimpleHttpServer listener;

        internal static GameModuleBase GameTelemetry;
        private String SelectedGame = "";

        private static string timeMsg, cityDest = "";
        private static double fuelamount, range, distance, speed, gas, rpm, temp, cruiseVal, dashLightState = 0;
        private static bool state_electricity, beaconState, lowBeamState, mainBeamState, parkingBrakeState, absState, trailerState, engineState, ccState, blinkerL, blinkerR, rearFogState, frontFogState, netState = false;

        private static int damageState = 0;

        //private static DateTime lastBlink, lastGearBlink;
        //private static int lastBlinkForce, lastGearBlinkForce = 0;


        private void button1_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Process.Start($"http://127.0.0.1:{_Config.HttpPort}/ebolero.html");
        }

        private void button_manualtest_Click(object sender, EventArgs e)
        {
            // Load correct gaming module
            if (SelectedGame != "")
            {
                GameTelemetry.Dispose();
                button_ets2.BackColor = System.Drawing.Color.FromKnownColor(System.Drawing.KnownColor.Control);
            }
            GameTelemetry = new TestingGame();
            SelectedGame = "Testing";
            button_manualtest.BackColor = System.Drawing.Color.GreenYellow;
            TaskbarIcon.Icon = Properties.Resources.connected;
            TaskbarIcon.Text = "JsonBUS - connected to Manual Test telemetry";
        }

        private void button_ets2_Click(object sender, EventArgs e)
        {
            // Load correct gaming module
            if (SelectedGame != "")
            {
                GameTelemetry.Dispose();
                button_manualtest.BackColor = System.Drawing.Color.FromKnownColor(System.Drawing.KnownColor.Control);
            }
            GameTelemetry = new ETS2game();
            SelectedGame = "ETS2";
            button_ets2.BackColor = System.Drawing.Color.GreenYellow;
            TaskbarIcon.Icon = Properties.Resources.connected;
            TaskbarIcon.Text = "JsonBUS - connected to ETS2 telemetry";

            MessageBox.Show("Euro Truck Simulator 2 telemetry selected.", "", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        internal ManagerForm()
        {
            InitializeComponent();

            // Initialize config object
            _Config = new Config();
            Ports = new List<SerialPort>() { };

            // Open all configured serial ports
            foreach (string port in SerialPort.GetPortNames())
            {
                if(_Config.SerialPorts.Contains(port))
                {
                    Ports.Add(new SerialPort(port, 115200));
                }
            }

            // set http response to active when listening enabled in config file
            if(_Config.HttpListen)
            {
                listener = new SimpleHttpServer(_Config.HttpPort);
                this.Load += listener.ListenToHttp;
            }

            //pushJsonPeriodically = new BackgroundWorker();
            //pushJsonPeriodically.DoWork += PushTelemetryToSerial;
            //pushJsonPeriodically.WorkerSupportsCancellation = true;
            //pushJsonPeriodically.WorkerReportsProgress = false;
            //pushJsonPeriodically.RunWorkerAsync();
        }


        internal static void PushTelemetry() {
            /*try
            {
                if (GameTelemetry.GameRunning)
                {
                    string jsonTelemetryObj = JsonConvert.SerializeObject(GameTelemetry);
                    WriteLineAllSerialPorts(jsonTelemetryObj);
                }
            }
            catch
            {

            }*/


            try {
                // Open Serial port to communicate with JsonBUS client
                foreach (var Port in Ports)
                {
                    if (!Port.IsOpen)
                    {
                        Port.Open();
                    }

                    if (GameTelemetry.GameRunning)
                    {

                        // Blinkers
                        var blinkrLNow = GameTelemetry.BlinkerL;
                        var blinkrRNow = GameTelemetry.BlinkerR;
                        if (blinkrLNow != blinkerL || blinkrRNow != blinkerR)
                        {
                            blinkerL = blinkrLNow;
                            blinkerR = blinkrRNow;
                            if (blinkerL && !blinkerR && state_electricity)
                            {
                                WriteLineAllSerialPorts("{\"f\":240,\"v\":1}");
                            }
                            else if (!blinkerL && blinkerR && state_electricity)
                            {
                                WriteLineAllSerialPorts("{\"f\":240,\"v\":2}");
                            }
                            else if (blinkerL && blinkerR)
                            {
                                WriteLineAllSerialPorts("{\"f\":240,\"v\":3}");
                            }
                            else
                            {
                                WriteLineAllSerialPorts("{\"f\":240,\"v\":0}");
                            }
                        }

                        var mainBeamNow = GameTelemetry.MainBeam;
                        if (mainBeamState != mainBeamNow)
                        {
                            mainBeamState = mainBeamNow;
                            WriteLineAllSerialPorts("{\"f\":238,\"v\":" + (mainBeamState ? "1" : "0") + "}");
                        }

                        // Electric enabled
                        var electricityNow = GameTelemetry.ElectricityOn;
                        if (state_electricity != electricityNow)
                        {
                            state_electricity = electricityNow;

                            if (!state_electricity)
                            {
                                beaconState = false;
                                dashLightState = 0;
                                lowBeamState = false;
                                parkingBrakeState = false;
                                absState = false;
                                trailerState = false;
                                engineState = false;
                                ccState = false;
                                fuelamount = 0;
                                range = 0;
                                distance = 0;
                                speed = 0;
                                gas = 0;
                                cruiseVal = 0;
                                cityDest = "";
                            }
                            else
                            {
                                WriteLineAllSerialPorts("{\"f\":239,\"v\":" + 255 + "}");
                                WriteLineAllSerialPorts("{\"f\":233,\"v\":" + GameTelemetry.WaterTemp + "}");
                                WriteLineAllSerialPorts("{\"f\":234,\"v\":" + GameTelemetry.FuelAmountPercent + "}");
                            }
                            WriteLineAllSerialPorts("{\"f\":230,\"v\":" + (state_electricity ? "1" : "0") + "}");
                        }

                        if (state_electricity)
                        {
                            // Gas
                            var tempActual = GameTelemetry.WaterTemp;
                            if (temp != tempActual && GameTelemetry.ElectricityOn)
                            {
                                temp = tempActual;
                                WriteLineAllSerialPorts("{\"f\":233,\"v\":" + tempActual + "}");
                            }

                            var gasLevelPercentage = GameTelemetry.FuelAmountPercent;
                            if (gas != gasLevelPercentage && GameTelemetry.ElectricityOn)
                            {
                                gas = gasLevelPercentage;
                                WriteLineAllSerialPorts("{\"f\":234,\"v\":" + gasLevelPercentage + "}");
                            }
                            // Engine enabled
                            var engineNow = GameTelemetry.Engine;
                            if (engineState != engineNow)
                            {
                                engineState = engineNow;
                                WriteLineAllSerialPorts("{\"f\":231,\"v\":" + (engineState ? "1" : "0") + "}");
                                Thread.Sleep(100);
                            }

                            if (engineNow)
                            {
                                // Damage report
                                int damageNow = GameTelemetry.DamageState;
                                if (damageState != damageNow)
                                {
                                    damageState = damageNow;
                                    WriteLineAllSerialPorts("{\"f\":246,\"v\":" + damageState + "}");
                                }

                                // Lights
                                var dashLightNow = GameTelemetry.DashLightLevel;
                                if (dashLightState != dashLightNow)
                                {
                                    dashLightState = dashLightNow;
                                    WriteLineAllSerialPorts("{\"f\":239,\"v\":" + dashLightState.ToString() + "}");
                                }

                                var lowBeamNow = GameTelemetry.LowBeam;
                                if (lowBeamState != lowBeamNow)
                                {
                                    lowBeamState = lowBeamNow;
                                    WriteLineAllSerialPorts("{\"f\":237,\"v\":" + (lowBeamState ? "1" : "0") + "}");
                                }

                                var absNow = GameTelemetry.Abs;
                                if (absState != absNow)
                                {
                                    absState = absNow;
                                    WriteLineAllSerialPorts("{\"f\":245,\"v\":" + (absState ? "1" : "0") + "}");
                                }

                                var parkingBrakeNow = GameTelemetry.ParkingBrake;
                                if (parkingBrakeState != parkingBrakeNow)
                                {
                                    parkingBrakeState = parkingBrakeNow;
                                    WriteLineAllSerialPorts("{\"f\":244,\"v\":" + (parkingBrakeState ? "1" : "0") + "}");
                                }

                                var trailerNow = GameTelemetry.Trailer;
                                if (trailerState != trailerNow)
                                {
                                    trailerState = trailerNow;
                                    WriteLineAllSerialPorts("{\"f\":249,\"v\":" + (trailerState ? "1" : "0") + "}");
                                }


                                var beaconNow = GameTelemetry.Beacon;
                                if (beaconState != beaconNow)
                                {
                                    beaconState = beaconNow;
                                    WriteLineAllSerialPorts("{\"f\":250,\"v\":" + (beaconState ? "1" : "0") + "}");
                                }

                                var frontFogNow = GameTelemetry.FogFront;
                                if (frontFogState != frontFogNow)
                                {
                                    frontFogState = frontFogNow;
                                    WriteLineAllSerialPorts("{\"f\":241,\"v\":" + (frontFogState ? "1" : "0") + "}");
                                }

                                var rearFogNow = GameTelemetry.FogRear;
                                if (rearFogState != rearFogNow)
                                {
                                    rearFogState = rearFogNow;
                                    WriteLineAllSerialPorts("{\"f\":242,\"v\":" + (rearFogState ? "1" : "0") + "}");
                                }

                                //var netNow = new Ping().Send("www.google.com.mx").Status == IPStatus.Success;
                                //if (netState != netNow)
                                //{
                                //    netState = netNow;
                                //    // if net not available, turn on oil pressure icon
                                //    WriteLineAllSerialPorts("{\"f\":247,\"v\":" + (netState ? "0" : "1") + "}");
                                //}

                                // GAUGES
                                var rpmActual = GameTelemetry.RPM;
                                if (rpm != rpmActual && GameTelemetry.Engine)
                                {
                                    rpm = rpmActual;
                                    WriteLineAllSerialPorts("{\"f\":235,\"v\":" + rpmActual + "}");
                                }

                                var speedActual = GameTelemetry.Speedometer;
                                if (speed != speedActual && GameTelemetry.Engine)
                                {
                                    speed = speedActual;
                                    WriteLineAllSerialPorts("{\"f\":236,\"v\":" + (speedActual + 2) + "}");
                                    WriteLineAllSerialPorts("{\"f\":235,\"v\":" + rpmActual + "}");
                                }

                                // Displays
                                // LEFT for time
                                string gearSelected = GameTelemetry.GearSelected < 0 ? "R" : GameTelemetry.GearSelected.ToString();
                                gearSelected = gearSelected == "0" ? "N" : gearSelected;
                                
                                gearSelected = (rpm > 4600 && !GameTelemetry.ForwardGearCountReached) ? "UP" : gearSelected;
                                gearSelected = (rpm < 1500 && GameTelemetry.GearSelected > 1) ? "DOWN" : gearSelected;

                                var timeMsgNew = $"{GameTelemetry.TimeMessage} {gearSelected}";
                                if (timeMsgNew != timeMsg)
                                {
                                    timeMsg = timeMsgNew;
                                    WriteLineAllSerialPorts("{\"f\":251,\"v\":1,\"t\":\"" + timeMsg + "\"}");
                                }

                                //// RIGHT for info
                                var distNew = GameTelemetry.TripDistance;
                                var cityNew = GameTelemetry.TripDestination;
                                if (distance != distNew || cityDest != cityNew)
                                {
                                    distance = distNew;
                                    cityDest = cityNew;

                                    var translited = Transliteration.CyrillicToLatin(cityDest);
                                    translited = translited.RemoveDiacritics();

                                    if (!String.IsNullOrEmpty(translited) && translited.Length > 16) translited = translited.Substring(0, 16);
                                    if (String.IsNullOrEmpty(translited)) translited = "NULL";

                                    WriteLineAllSerialPorts("{\"f\":252,\"v\":" + distance + ",\"t\":\"" + translited + "\"}");
                                }

                                var ccNow = GameTelemetry.CruiseControl;
                                var cruiseValNow = Convert.ToInt32(GameTelemetry.CruiseControlValue);
                                if (cruiseValNow % 10 == 1 || cruiseValNow % 10 == 6)
                                {
                                    cruiseValNow--;
                                }

                                if (ccState != ccNow || cruiseVal != cruiseValNow)
                                {
                                    ccState = ccNow;
                                    cruiseVal = cruiseValNow;
                                    string cruiseValText = $"{cruiseValNow}";
                                    WriteLineAllSerialPorts("{\"f\":253,\"v\":" + (ccState ? cruiseValText : "0") + "}");
                                }

                                var fuelamountNew = GameTelemetry.FuelAmountLitres;
                                var rangeNew = GameTelemetry.FuelRangeKm;
                                if (fuelamount != fuelamountNew || range != rangeNew)
                                {
                                    fuelamount = fuelamountNew;
                                    range = rangeNew;
                                    WriteLineAllSerialPorts("{\"f\":255,\"v\":" + range + "}");
                                    WriteLineAllSerialPorts("{\"f\":254,\"v\":" + fuelamount + "}");
                                }

                                //if(speed > (GameTelemetry.SpeedLimit + 5) && GameTelemetry.SpeedLimit > 0)
                                //{
                                //    if (DateTime.Compare(DateTime.Now, lastBlink.AddSeconds(0.3)) >= 0)
                                //    {
                                //        lastBlink = DateTime.Now;
                                //        WriteLineAllSerialPorts("{\"f\":256,\"v\":" + lastBlinkForce.ToString() + "}");
                                //        lastBlinkForce = (lastBlinkForce == 0 ? 1 : 0);
                                //    }
                                //}
                                //else
                                //{
                                //    WriteLineAllSerialPorts("{\"f\":256,\"v\":0}");
                                //}

                                //if (gearSelected == "UP" || gearSelected == "DOWN")                                {
                                //    if (DateTime.Compare(DateTime.Now, lastGearBlink.AddSeconds(0.5)) >= 0)
                                //    {
                                //        lastGearBlink = DateTime.Now;
                                //        WriteLineAllSerialPorts("{\"f\":257,\"v\":" + lastGearBlinkForce.ToString() + "}");
                                //        lastGearBlinkForce = (lastGearBlinkForce == 0 ? 1 : 0);
                                //    }
                                //}
                                //else
                                //{
                                //    WriteLineAllSerialPorts("{\"f\":257,\"v\":0}");
                                //}

                            }
                        }

                    }
                    //else if(!data.SdkActive)
                    else if (!GameTelemetry.GameRunning)
                    {
                        Thread.Sleep(10);
                        WriteLineAllSerialPorts("{\"f\":230,\"v\":0}");
                    }

                }
            }

            catch (Exception ex)
            {
                // ignored atm i found no proper way to shut the telemetry down and down call this anymore when this or another thing is already disposed
                Console.WriteLine("Telemetry was closed: " + ex);
            }
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            Thread.Sleep(10);
            WriteLineAllSerialPorts("{\"f\":230,\"v\":0}");
            listener.Stop();
            foreach (var Port in Ports)
            {
                if (Port.IsOpen)
                {
                    Port.Close();
                }
            }
        }

        internal static void WriteLineAllSerialPorts(string value)
        {
            foreach(var port in Ports)
            {
                if (!port.IsOpen) port.Open();               
                if(port.IsOpen) port.WriteLine(value);
            }
        }

    }
}
