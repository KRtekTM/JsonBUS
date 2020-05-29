using SCSSdkClient;
using SCSSdkClient.Object;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace JsonBUS.manager.TelemetryModules
{
    public class ETS2game : GameModuleBase
    {
        private SCSTelemetry _data;
        private SCSSdkTelemetry EtsTelemetry;

        // control used to invoke if needed
        private static Control InvokeControl;

        public ETS2game()
        {
            InvokeControl = new Control();
            InvokeControl.CreateControl();

            EtsTelemetry = new SCSSdkTelemetry();
            EtsTelemetry.Data += EtsTelemetry_Data;
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


        // HTTP server messages
        public override string HttpString
        {
            get
            {
                if (!_data.SdkActive)
                {
                    return "{\"gameTime\":\"NOT RUNNING\", \"simulationOn\":\"FALSE\", \"electricityOn\":\"FALSE\", \"gameVersion\":\"\"}";
                }
                else
                {
                    if (!ElectricityOn)
                    {
                        return "{\"gameTime\":\"NOT RUNNING\", \"simulationOn\":\"TRUE\", \"electricityOn\":\"FALSE\", \"gameVersion\":\"" + _data.GameVersion.ToString() + "\"}";
                    }
                    else
                    {
                        var overspeed = ((_data.TruckValues.CurrentValues.DashboardValues.Speed.Kph > _data.NavigationValues.SpeedLimit.Kph && _data.NavigationValues.SpeedLimit.Kph > 0) ? "TRUE" : "FALSE");
                        return "{\"gameTime\":\"" + _data.CommonValues.GameTime.Date.ToString("HH:mm") + "\", \"simulationOn\":\"TRUE\", \"electricityOn\":\"TRUE\", \"gameVersion\":\"" + _data.GameVersion.ToString() + "\", \"overspeed\":\"" + overspeed + "\"}";
                    }
                }
            }
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
        public override double DashLightLevel => (_data.TruckValues.CurrentValues.LightsValues.DashboardBacklight * 100);

        // Fuel values
        public override double FuelAmountLitres => Math.Ceiling(_data.TruckValues.CurrentValues.DashboardValues.FuelValue.Amount);
        public override double FuelRangeKm => Math.Ceiling(_data.TruckValues.CurrentValues.DashboardValues.FuelValue.Range);
        public override double FuelAmountPercent => Math.Ceiling((_data.TruckValues.CurrentValues.DashboardValues.FuelValue.Amount / _data.TruckValues.ConstantsValues.CapacityValues.Fuel) * 100);

        // Gauges
        public override double Speedometer => Math.Ceiling(_data.TruckValues.CurrentValues.DashboardValues.Speed.Kph);
        public override double RPM => Math.Ceiling(_data.TruckValues.CurrentValues.DashboardValues.RPM);
        public override double WaterTemp => Math.Ceiling(_data.TruckValues.CurrentValues.DashboardValues.WaterTemperature);

        // Other
        public override int DamageState => (int)Math.Ceiling(_data.TruckValues.CurrentValues.DamageValues.Chassis * 100);
        public override string TimeMessage => _data.CommonValues.GameTime.Date.ToString("HH:mm");
        public override string TripDestination => _data.JobValues.CityDestination;
        public override double TripDistance => (Math.Round(_data.NavigationValues.NavigationDistance / 1000, 1) * 10); //source is in meters, we send km with precision on 1 decimal
        public override bool CruiseControl => _data.TruckValues.CurrentValues.DashboardValues.CruiseControl;
        public override double CruiseControlValue => Math.Ceiling(_data.TruckValues.CurrentValues.DashboardValues.CruiseControlSpeed.Kph);
    }
}
