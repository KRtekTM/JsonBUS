using System;
using System.ComponentModel;

namespace JsonBUS.manager.TelemetryModules
{
    public class TestingGame : GameModuleBase
    {
        private ManualTest manualTest;
        private bool electricityState = false;
        private BackgroundWorker worker;
        private bool blinkNow = false;
        private int blinkControl = 0;
        private int fuelAmount = 1000;
        private int speed = 0;
        private int rpm = 0;
        private int temp = 0;

        public TestingGame()
        {
            manualTest = new ManualTest();
            manualTest.Show();
            electricityState = true;

            worker = new BackgroundWorker();

            worker.DoWork += Worker_DoWork;
            worker.WorkerSupportsCancellation = true;
            worker.WorkerReportsProgress = false;
            worker.RunWorkerAsync();
        }

        private void Worker_DoWork(object sender, DoWorkEventArgs e)
        {
            while(true)
            {
                ManagerForm.PushTelemetry();

                if(blinkControl >= 35)
                {
                    // Continuously decrease fuel level, when droped below 10 restart
                    if (fuelAmount <= 20)
                    {
                        fuelAmount = 1000;
                        speed = 0;
                        rpm = 0;
                        temp = 0;
                    }
                    else
                    {
                        fuelAmount = fuelAmount - 15;
                        if (speed >= 220)
                        {
                            speed = 0;
                        }
                        if (rpm >= 5000)
                        {
                            rpm = 0;
                        }
                        if (temp > 110)
                        {
                            temp = 0;
                        }

                        
                        speed++;
                        rpm++;
                        temp++;

                    }

                    blinkNow = blinkNow ? false : true;
                    manualTest.leftIndicator.BackgroundImage = (manualTest.leftIndicator.Checked && blinkNow) ? Properties.Resources.leftT : Properties.Resources.left;
                    manualTest.rightIndicator.BackgroundImage = (manualTest.rightIndicator.Checked && blinkNow) ? Properties.Resources.rightT : Properties.Resources.right;
                    blinkControl = 0;
                }

                blinkControl++;

                System.Threading.Thread.Sleep(10);
            }
        }

        public override void Dispose()
        {
            electricityState = false;

            worker.CancelAsync();
            worker.Dispose();

            manualTest.Close();
            manualTest.Dispose();
        }

        // General
        public override bool GameRunning => true;
        public override bool ElectricityOn => electricityState;
        public override bool Engine => manualTest.engine.Checked;

        // Lights
        public override bool BlinkerL => manualTest.leftIndicator.Checked && blinkNow;
        public override bool BlinkerR => manualTest.rightIndicator.Checked && blinkNow;
        public override bool Beacon => false;
        public override bool LowBeam => false;
        public override bool MainBeam => manualTest.highBeam.Checked;
        public override bool ParkingBrake => false;
        public override bool Abs => false;
        public override bool Trailer => false;
        public override double DashLightLevel => 100;

        // Fuel values
        public override double FuelAmountLitres => fuelAmount;
        public override double FuelRangeKm => fuelAmount * 1.5;
        public override double FuelAmountPercent => fuelAmount/4;

        // Gauges
        public override double Speedometer => speed;
        public override double RPM => rpm;
        public override double WaterTemp => temp;

        // Other
        public override int DamageState => 0;
        public override string TimeMessage => DateTime.Now.ToString("HH:mm");
        public override string TripDestination => "";
        public override double TripDistance => 0;
        public override bool CruiseControl => false;
        public override double CruiseControlValue => 0;

        public override bool Airbag => false;
        public override bool FogFront => false;
        public override bool FogRear => false;
        public override bool OilWarn => false;
        public override bool EngineWarn => false;
        public override bool Failure => false;
        public override bool Immobilizer => false;
        public override bool SafeWarn => false;
        public override bool Speaker => false;
    }
}
