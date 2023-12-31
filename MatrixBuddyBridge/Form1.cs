using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Windows.Forms;
using System.IO.Ports;
using System.Timers;
using Timer = System.Timers.Timer;
//using CSCore.CoreAudioAPI;
using CoreAudio;
using CoreAudio.Interfaces;

namespace MatrixBuddyBridge
{
    public partial class Form1 : Form
    {
        private enum MouthStates
        {
            Rest = 0, // this is the state when the box has been released from control override
            Closed = 1, // this is a closed neutral mouth, uses Yawn1
            Talk = 2, // talking, uses Talk
            Shout = 3, // shouting, uses Yawn3
            Screm = 4, // Max open, uses Yawn4  -  Closes eyes
        }

        private MouthStates _mouthState = MouthStates.Rest;
        
        private SerialPort _serialPort;
        private bool _mouthOpen;
        private Stopwatch _stopwatch = new Stopwatch();

        private RenderDevice _renderDevice;
        private MMDevice _mmDevice;
        private SessionCollection _sessions;
        private AudioSessionControl2 _session;
        private List<float> _prevSamples = new List<float>();

        private const int _sampleCount = 4;
        
        public Form1()
        {
            InitializeComponent();
            
            _serialPort = new SerialPort();
            _serialPort.BaudRate = 57600;
            _serialPort.ReadTimeout = 500;
            
            //ScanSerial();
            ScanAudioSources();
            
            _stopwatch.Start();
            
            
            Timer tmr = new Timer();
            tmr.Interval = 100;   // milliseconds
            tmr.Elapsed += SerialUpdate;  // set handler
            tmr.Elapsed += VUBarUpdate;
            tmr.Start();
        }

        private void SerialUpdate(object sender, ElapsedEventArgs elapsedEventArgs)
        {
            if (!muteCheckBox.Checked && _serialPort.IsOpen && 
                _session != null && _session.AudioMeterInformation != null)
            {
                float sample = _session.AudioMeterInformation.MasterPeakValue;
                if (_prevSamples.Count == 0)
                {
                    _prevSamples.Add(sample);
                }
                

                if (sample == 0)
                {
                    _serialPort.Write("0");
                }
                else if (_prevSamples.Average() < trackBar1.Value / 100.0) // talk mode
                {
                    _mouthOpen = false;
                    if (sample > _prevSamples.Average())
                    {
                        _serialPort.Write("2");
                    }
                    else
                    {
                        _serialPort.Write("1");
                    }
                }
                else // sing mode
                {
                    _mouthOpen = true;
                    if (sample < _prevSamples.Average() - 0.3)
                    {
                        _serialPort.Write("1");
                    }
                    else if (sample > 0.7f && sample > _prevSamples.Average())
                    {
                        _serialPort.Write("4");
                    }
                    else
                    {
                        _serialPort.Write("3");
                    }
                }

                //_mouthOpen = !_mouthOpen;
                _prevSamples.Insert(0, sample);
                if (_prevSamples.Count > _sampleCount)
                {
                    _prevSamples.RemoveRange(_sampleCount, _prevSamples.Count - _sampleCount);
                }
            }
            else
            {
                _mouthState = MouthStates.Rest;
                _prevSamples.Clear();
            }
        }

        private void VUBarUpdate(object sender, ElapsedEventArgs elapsedEventArgs)
        {
            if (_session != null && _session.AudioMeterInformation != null)
            {
                progressBar1.Value = (int)(Math.Round(_session.AudioMeterInformation.MasterPeakValue * 100));
            }
            else
            {
                progressBar1.Value = 50;
            }
        }

        private void ScanSerial()
        {
            Console.WriteLine($"Scanning {SerialPort.GetPortNames().Length} ports...");
            ScanResult.Text = $"Scanning {SerialPort.GetPortNames().Length} ports...";
            
            for (int i = 0; i < 10; i++)
            {
                foreach (string portName in SerialPort.GetPortNames())
                {
                    Console.WriteLine($" - {portName} attempt {i}");
                    if (_serialPort.IsOpen)
                    {
                        _serialPort.Close();
                        Thread.Sleep(50);
                    }
                    _serialPort.PortName = portName;
                    try
                    {
                        _serialPort.Open();
                    
                        _serialPort.Write("s");
                    
                        Thread.Sleep(100);
                    
                        if (_serialPort.ReadChar() == 'a')
                        {
                            ScanResult.Text = $"Connected on {portName}";
                            Console.WriteLine($"Connected on {portName}");
                            return;
                        }
                    }
                    catch (Exception exception) { }
                }
            }
            
            _serialPort.Close();
            ScanResult.Text = "Could not find device";
            Console.WriteLine("Could not find device");
        }


        private void ScanAudioSources()
        {
            List<RenderDevice> rDevs = new List<RenderDevice>();
            MMDeviceEnumerator deviceEnumerator = new MMDeviceEnumerator(Guid.NewGuid());
            MMDeviceCollection devCol = deviceEnumerator.EnumerateAudioEndPoints(DataFlow.Render, DeviceState.Active);
            int selectedIndex = 0;
            
            for(int i = 0; i < devCol.Count; i++) {
                RenderDevice rdev = new RenderDevice(devCol[i]);
                if(rdev.Device.Selected) selectedIndex = i;
                rDevs.Add(rdev);
            }

            if(rDevs.Count > 0) _renderDevice = rDevs[selectedIndex];
            _mmDevice = _renderDevice.Device;
            
            _sessions = _mmDevice.AudioSessionManager2.Sessions;
            
            AudioSelector.Items.Clear();

            foreach (AudioSessionControl2 session in _sessions)
            {
                Process p = Process.GetProcessById((int)session.ProcessID);
                string name = session.IsSystemSoundsSession ? "System Sounds" : session.DisplayName;
                if (name == "") name = p.ProcessName;

                AudioSelector.Items.Add(name);
            }
        }

        private void ScanButton_Click(object sender, EventArgs e)
        {
            ScanSerial();
        }

        private void AudioSelector_DropDown(object sender, EventArgs e)
        {
            ScanAudioSources();
        }

        private void AudioSelector_SelectedIndexChanged(object sender, EventArgs e)
        {
            _session = _sessions[AudioSelector.SelectedIndex];
        }

        #region stolen from coreAudioForms.Sessions

        private class RenderDevice {
            public readonly string Name;
            public readonly MMDevice Device;
        
            public RenderDevice(MMDevice device) {
                Device = device;
                Name = $"{device.Properties[PKey.DeviceDescription].Value} ({device.DeviceInterfaceFriendlyName})";
            }
        
            public override string ToString() {
                return Name;
            }
        }

        #endregion
    }
}