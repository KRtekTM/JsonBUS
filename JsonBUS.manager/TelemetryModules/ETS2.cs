using JsonBUS.manager.Helpers;
using NickBuhro.Translit;
using SCSSdkClient;
using SCSSdkClient.Object;
using System;
using System.ComponentModel;
using System.Windows.Forms;

namespace JsonBUS.manager.TelemetryModules
{
    public class ETS2game : GameModuleBase
    {
        private SCSTelemetry _data;
        private SCSSdkTelemetry EtsTelemetry;
        private BackgroundWorker pushJsonPeriodically;

        // control used to invoke if needed
        private static Control InvokeControl;

        public ETS2game()
        {
            InvokeControl = new Control();
            InvokeControl.CreateControl();

            EtsTelemetry = new SCSSdkTelemetry();
            EtsTelemetry.Data += EtsTelemetry_Data;

                pushJsonPeriodically = new BackgroundWorker();
                pushJsonPeriodically.DoWork += PushTelemetryToSerial;
                pushJsonPeriodically.WorkerSupportsCancellation = true;
                pushJsonPeriodically.WorkerReportsProgress = false;
                pushJsonPeriodically.RunWorkerAsync();
        }

        private void PushTelemetryToSerial(object sender, DoWorkEventArgs e)
        {
            InvokeControl.Invoke(new TelemetryData(EtsTelemetry_Data), _data, true);
            //GameTelemetry = new ETS2game(data);
            ManagerForm.PushTelemetry();
            //System.Threading.Thread.Sleep(1);
        }

        public override void Dispose()
        {
            EtsTelemetry.Dispose();
            InvokeControl.Dispose();
        }

        private void EtsTelemetry_Data(SCSTelemetry data, bool updated)
        {
            try
            {

                // Invoke telemetry collection
                if (InvokeControl.InvokeRequired)
                {
                InvokeControl.Invoke(new TelemetryData(EtsTelemetry_Data), data, updated);
                    //GameTelemetry = new ETS2game(data);
                    _data = data;
                    ManagerForm.PushTelemetry();
                    return;
                }
            }
            catch { }
        }

        // General
        public override bool GameRunning => _data.SdkActive;// && !_data.Paused;
        public override bool ElectricityOn => _data.TruckValues.CurrentValues.ElectricEnabled;
        public override bool Engine => _data.TruckValues.CurrentValues.EngineEnabled;

        // Lights
        public override bool BlinkerL => _data.TruckValues.CurrentValues.LightsValues.BlinkerLeftOn;
        public override bool BlinkerR => _data.TruckValues.CurrentValues.LightsValues.BlinkerRightOn;
        public override bool Beacon => _data.TruckValues.CurrentValues.LightsValues.Beacon;
        public override bool LowBeam => _data.TruckValues.CurrentValues.LightsValues.BeamLow;
        public override bool MainBeam => (_data.TruckValues.CurrentValues.LightsValues.BeamHigh || _data.TruckValues.CurrentValues.LightsValues.AuxFront == AuxLevel.Full);
        public override bool ParkingBrake => _data.TruckValues.CurrentValues.MotorValues.BrakeValues.ParkingBrake;
        public override bool Abs => (_data.TruckValues.CurrentValues.LightsValues.Brake && !_data.TruckValues.CurrentValues.MotorValues.BrakeValues.ParkingBrake);
        public override bool Trailer => _data.TrailerValues[0].Attached;
        public override double DashLightLevel => (_data.TruckValues.CurrentValues.LightsValues.DashboardBacklight * 255);
        public override bool Airbag => false;
        public override bool FogFront => _data.TruckValues.CurrentValues.DashboardValues.GearDashboards;
        public override bool FogRear => _data.TruckValues.CurrentValues.LightsValues.Reverse;
        public override bool OilWarn => false;
        public override bool EngineWarn => false;
        public override bool Failure => false;
        public override bool Immobilizer => false;
        public override bool SafeWarn => false;
        public override bool Speaker => false;

        // Fuel values
        public override double FuelAmountLitres => Math.Ceiling(_data.TruckValues.CurrentValues.DashboardValues.FuelValue.Amount);
        public override double FuelRangeKm => Math.Ceiling(_data.TruckValues.CurrentValues.DashboardValues.FuelValue.Range);
        public override double FuelAmountPercent => Math.Ceiling((_data.TruckValues.CurrentValues.DashboardValues.FuelValue.Amount / _data.TruckValues.ConstantsValues.CapacityValues.Fuel) * 100);

        // Gauges
        public override double Speedometer => Math.Abs(Math.Ceiling(_data.TruckValues.CurrentValues.DashboardValues.Speed.Kph));
        public override double RPM => Math.Ceiling(_data.TruckValues.CurrentValues.DashboardValues.RPM) * 2.5; // correction for diesel engines because my cluster instrument is for petrol engines
        public override double WaterTemp => Math.Ceiling(_data.TruckValues.CurrentValues.DashboardValues.WaterTemperature);

        // Other
        public override int DamageState => (int)Math.Ceiling(_data.TruckValues.CurrentValues.DamageValues.Chassis * 100);
        public override string TimeMessage => _data.CommonValues.GameTime.Date.ToString("HH:mm");
        public override string TripDestination => Transliteration.CyrillicToLatin(_data.JobValues.CityDestination).RemoveDiacritics();
        public override double TripDistance => (Math.Round(_data.NavigationValues.NavigationDistance / 1000, 1) * 10); //source is in meters, we send km with precision on 1 decimal
        public override bool CruiseControl => _data.TruckValues.CurrentValues.DashboardValues.CruiseControl;
        public override double CruiseControlValue => Math.Ceiling(_data.TruckValues.CurrentValues.DashboardValues.CruiseControlSpeed.Kph);
    }
}
