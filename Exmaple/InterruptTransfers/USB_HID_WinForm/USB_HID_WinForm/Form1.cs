using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using USB;

namespace USB_HID_WinForm {
    public partial class Form1 : Form {

        public Form1() {
            InitializeComponent();

            Thread thread = new Thread(USB_ReadThread);
            thread.IsBackground = true;
            thread.Start();
        }

        private void send_button_Click(object sender, EventArgs e) {
            if (textBox1.Text == "")
                return;

            USBDevice USB = new USBDevice(0x1234, 0x5678);
            if (!USB.Connect()) {
                MessageBox.Show("Unable to connect to the USB device!");
                return;
            }

            byte[] buffer = Encoding.ASCII.GetBytes(textBox1.Text);

            if (USB.Write(buffer))
                richTextBox1.Text += Encoding.ASCII.GetString(buffer) + "\r\n";
            else 
                MessageBox.Show("Send error data!");
        }

        private void USB_ReadThread() {
            USBDevice USB = new USBDevice(0x1234, 0x5678);
            while (true) {
                byte[] data = USB.Read();
                if (data == null) {
                    USB.Connect();
                    Thread.Sleep(500);
                }
                else {
                    this.BeginInvoke((MethodInvoker)delegate () {
                        richTextBox2.Text += Encoding.ASCII.GetString(data) + "\r\n";
                        richTextBox2.SelectionStart = richTextBox2.Text.Length - 1;
                    });
                    Thread.Sleep(4);
                }
            }
        }
    }
}
