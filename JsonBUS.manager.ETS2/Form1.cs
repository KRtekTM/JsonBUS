using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Windows.Forms;
using SCSSdkClient;
using SCSSdkClient.Object;
using System.Threading;

namespace JsonBUS.manager.ETS2
{
    public partial class Form1 : Form
    {
        public SCSSdkTelemetry Telemetry;
        public SerialPort Port;

        public Form1()
        {
            InitializeComponent();

            //foreach(String port in SerialPort.GetPortNames())
            //{
            //    listView1.Items.Add(port);
            //}

            Port = new SerialPort("COM8", 115200);

            Telemetry = new SCSSdkTelemetry();
            Telemetry.Data += Telemetry_Data;

        }


        private bool state_electricity = false;

        private void Telemetry_Data(SCSTelemetry data, bool updated)
        {
            try
            {
                // Invoke telemetry collection
                if (InvokeRequired)
                {
                    Invoke(new TelemetryData(Telemetry_Data), data, updated);
                    return;
                }
                
                // Open Serial port to communicate with JsonBUS client
                if (!Port.IsOpen)
                {
                    Port.Open();
                }

                if (data.SdkActive && !data.Paused)
                {


                    // Blinkers
                    if (data.TruckValues.CurrentValues.LightsValues.BlinkerLeftOn == true && data.TruckValues.CurrentValues.LightsValues.BlinkerRightOn == false) Port.WriteLine("{\"f\":240,\"v\":1}");
                    else if (data.TruckValues.CurrentValues.LightsValues.BlinkerLeftOn == false && data.TruckValues.CurrentValues.LightsValues.BlinkerRightOn == true) Port.WriteLine("{\"f\":240,\"v\":2}");
                    else if (data.TruckValues.CurrentValues.LightsValues.BlinkerLeftOn == true && data.TruckValues.CurrentValues.LightsValues.BlinkerRightOn == true) Port.WriteLine("{\"f\":240,\"v\":3}");
                    else Port.WriteLine("{\"f\":240,\"v\":0}");

                    //Thread.Sleep(1);

                    // Electric enabled
                    if (state_electricity != data.TruckValues.CurrentValues.ElectricEnabled)
                    {
                        if (data.TruckValues.CurrentValues.ElectricEnabled)
                        {
                            Port.WriteLine("{\"f\":230,\"v\":1}");
                        }
                        else
                        {
                            Port.WriteLine("{\"f\":230,\"v\":0}");
                        }
                        state_electricity = data.TruckValues.CurrentValues.ElectricEnabled;
                    }

                    //Thread.Sleep(1);

                    // Engine enabled
                    if (data.TruckValues.CurrentValues.EngineEnabled)
                    {
                        Port.WriteLine("{\"f\":231,\"v\":1}");
                    }
                    else
                    {
                        Port.WriteLine("{\"f\":231,\"v\":0}");
                    }

                    //Thread.Sleep(1);

                    // Damage report
                    int damage = (int)Math.Ceiling(data.TruckValues.CurrentValues.DamageValues.Chassis * 100);
                    Port.WriteLine("{\"f\":246,\"v\":" + damage + "}");
                    //Thread.Sleep(1);

                    // Lights
                    if (data.TruckValues.CurrentValues.LightsValues.Parking)
                    {
                        Port.WriteLine("{\"f\":239,\"v\":" + (data.TruckValues.CurrentValues.LightsValues.DashboardBacklight * 100) + "}");
                    }

                    if (data.TruckValues.CurrentValues.LightsValues.BeamLow)
                    {
                        Port.WriteLine("{\"f\":237,\"v\":1}");
                    }
                    else
                    {
                        Port.WriteLine("{\"f\":237,\"v\":0}");
                    }
                    //Thread.Sleep(1);

                    if (data.TruckValues.CurrentValues.LightsValues.BeamHigh || data.TruckValues.CurrentValues.LightsValues.AuxFront == AuxLevel.Full)
                    {
                        Port.WriteLine("{\"f\":238,\"v\":1}");
                    }
                    else
                    {
                        Port.WriteLine("{\"f\":238,\"v\":0}");
                    }
                    //Thread.Sleep(1);

                    if (data.TruckValues.CurrentValues.MotorValues.BrakeValues.ParkingBrake)
                    {
                        Port.WriteLine("{\"f\":244,\"v\":1}");
                    }
                    else
                    {
                        Port.WriteLine("{\"f\":244,\"v\":0}");
                    }
                    //Thread.Sleep(1);

                    if (data.TruckValues.CurrentValues.LightsValues.Brake && !data.TruckValues.CurrentValues.MotorValues.BrakeValues.ParkingBrake)
                    {
                        Port.WriteLine("{\"f\":245,\"v\":1}");
                    }
                    else
                    {
                        Port.WriteLine("{\"f\":245,\"v\":0}");
                    }
                    //Thread.Sleep(1);

                    if (data.TruckValues.CurrentValues.DashboardValues.CruiseControl)
                    {
                        Port.WriteLine("{\"f\":232,\"v\":1}");
                    }
                    else
                    {
                        Port.WriteLine("{\"f\":232,\"v\":0}");
                    }

                    if(data.TrailerValues[0].Attached)
                    {
                        Port.WriteLine("{\"f\":241,\"v\":1}");
                    }
                    else
                    {
                        Port.WriteLine("{\"f\":241,\"v\":0}");
                    }

                    // GAUGES
                    // Gas
                    var gasLevelPercentage = Math.Ceiling((data.TruckValues.CurrentValues.DashboardValues.FuelValue.Amount / data.TruckValues.ConstantsValues.CapacityValues.Fuel) * 100);
                    Port.WriteLine("{\"f\":234,\"v\":" + gasLevelPercentage + "}");
                    //Thread.Sleep(1);
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
            Port.WriteLine("{\"f\":230,\"v\":0}");
        }
    }
}
