using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using USB;

namespace USB_HID_WinForm {
    public partial class Form1 : Form {

        public Form1() {
            InitializeComponent();
        }

        private void send_button_Click(object sender, EventArgs e) {
            if (textBox1.Text == "")
                return;

            USBDevice USB = new USBDevice(0x1234, 0x5678);
            if (!USB.Connect()) {
                MessageBox.Show("Unable to connect to the USB device!");
                return;
            }

            int REPORTINPUT = 0x01;
            int REPORT7 = 0x02, REPORT15 = 0x03, REPORT31 = 0x04, REPORT63 = 0x05, REPORT127 = 0x06, REPORT247 = 0x07;

            byte[] buffer = Encoding.ASCII.GetBytes(textBox1.Text);
            bool result;

            if (buffer.Length <= 7)
                result = USB.WriteControl(REPORT7, buffer, 7);
            else if (buffer.Length <= 15)
                result = USB.WriteControl(REPORT15, buffer, 15);
            else if (buffer.Length <= 31)
                result = USB.WriteControl(REPORT31, buffer, 31);
            else if (buffer.Length <= 63)
                result = USB.WriteControl(REPORT63, buffer, 63);
            else if (buffer.Length <= 127)
                result = USB.WriteControl(REPORT127, buffer, 127);
            else if (buffer.Length <= 247)
                result = USB.WriteControl(REPORT247, buffer, 247);
            else {
                MessageBox.Show("No description of the report supports package sizes!");
                return;
            }

            if (result) {
                richTextBox1.Text += Encoding.ASCII.GetString(buffer) + "\r\n";
                byte[] data = USB.ReadControl(REPORTINPUT);
                if (data != null) {
                    string str = "";
                    for (int i = 0; i < data.Length; i++) {
                        if (data[i] == 0)
                            break;
                        str += (char)data[i];
                    }
                    richTextBox2.Text += str + "\r\n";
                }
            }
            else 
                MessageBox.Show("Send error data!");
        }
    }
}
