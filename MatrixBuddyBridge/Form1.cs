using System;
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
using CSCore.CoreAudioAPI;

namespace MatrixBuddyBridge
{
    public partial class Form1 : Form
    {
        private SerialPort _serialPort;
        private bool _mouthOpen;
        
        public Form1()
        {
            InitializeComponent();

            _serialPort = new SerialPort();
            _serialPort.BaudRate = 57600;
            
            ScanSerial();
            ScanAudioSources();
            
            Timer tmr = new Timer();
            tmr.Interval = 50;   // milliseconds
            tmr.Elapsed += SerialUpdate;  // set handler
            tmr.Start();
        }

        private void SerialUpdate(object sender, ElapsedEventArgs elapsedEventArgs)
        {
            if (!muteCheckBox.Checked && _serialPort.IsOpen)
            {
                if (_mouthOpen)
                {
                    _serialPort.Write("v");
                }
                else
                {
                    _serialPort.Write("o");
                }

                _mouthOpen = !_mouthOpen;
            }
        }

        private void ScanSerial()
        {
            Console.WriteLine($"Scanning {SerialPort.GetPortNames().Length} ports...");
            ScanResult.Text = $"Scanning {SerialPort.GetPortNames().Length} ports...";

            foreach (string portName in SerialPort.GetPortNames())
            {
                Console.WriteLine($" - {portName}");
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
            
            _serialPort.Close();
            ScanResult.Text = "Could not find device";
            Console.WriteLine("Could not find device");
        }

        private void ScanAudioSources()
        {
            
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
            throw new System.NotImplementedException();
        }
    }
}