namespace JsonBUS.manager.TelemetryModules
{
    public abstract class GameModuleBase
    {

        // General
        public abstract bool GameRunning
        {
            get;
        }

        public abstract bool ElectricityOn
        {
            get;
        }

        public abstract bool Engine
        {
            get;
        }

        // Lights
        public abstract bool BlinkerL
        {
            get;
        }

        public abstract bool BlinkerR
        {
            get;
        }

        public abstract bool Beacon
        {
            get;
        }

        public abstract bool LowBeam
        {
            get;
        }

        public abstract bool MainBeam
        {
            get;
        }

        public abstract bool ParkingBrake
        {
            get;
        }

        public abstract bool Abs
        {
            get;
        }

        public abstract bool Trailer
        {
            get;
        }

        public abstract double DashLightLevel
        {
            get;
        }

        // Fuel values
        public abstract double FuelAmountLitres
        {
            get;
        }

        public abstract double FuelRangeKm
        {
            get;
        }

        public abstract double FuelAmountPercent
        {
            get;
        }

        public abstract double TripDistance
        {
            get;
        }

        // Gauges
        public abstract double Speedometer
        {
            get;
        }

        public abstract double RPM
        {
            get;
        }

        public abstract double WaterTemp
        {
            get;
        }

        // Other
        public abstract int DamageState
        {
            get;
        }

        public abstract string TimeMessage
        {
            get;
        }

        public abstract string TripDestination
        {
            get;
        }

        public abstract bool CruiseControl
        {
            get;
        }

        public abstract double CruiseControlValue
        {
            get;
        }

        public abstract void Dispose();

        public abstract bool Airbag { get;}
        public abstract bool FogFront { get; }
        public abstract bool FogRear { get; }
        public abstract bool OilWarn { get; }
        public abstract bool EngineWarn { get;}
        public abstract bool Failure { get; }
        public abstract bool Immobilizer { get; }
        public abstract bool SafeWarn { get;  }
        public abstract bool Speaker { get; }
        public abstract double SpeedLimit { get; }
        public abstract bool ForwardGearCountReached { get; }
        public abstract int GearSelected { get; }
    }
}

