using KDS.e6502;
using System.Text;

namespace e6502Debugger
{
    public partial class MainForm : Form
    {
        private Cpu? cpu;

        public MainForm()
        {
            InitializeComponent();
            ClearScreen();
        }

        private void ClearScreen()
        {
            lblA.Text = "";
            lblX.Text = "";
            lblY.Text = "";
            lblSP.Text = "";
            lblPC.Text = "";
            lblFlags.Text = "";
            lblNextInstruction.Text = "";
            txtMemory.Text = "";
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            var dialog = new OpenFileDialog();
            if(dialog.ShowDialog() == DialogResult.OK)
            {
                LoadFile(dialog.FileName);
            }
        }

        private void LoadFile(string file)
        {
            var bus = new BusDevice(File.ReadAllBytes(file), 0xf000);
            cpu = new Cpu(bus, E6502Type.Nmos);
            UpdateScreen();
        }

        private void UpdateScreen()
        {
            if (cpu == null) return;

            lblA.Text = "";
            lblX.Text = "";
            lblY.Text = "";
            lblSP.Text = "";
            lblPC.Text = $"{cpu.Pc:X4}";
            lblFlags.Text = "";
            lblNextInstruction.Text = $"${cpu.Pc:X4}";

            UpdateMemory();
        }

        private void btnStep_Click(object sender, EventArgs e)
        {
            ExecuteNextInstruction();
        }

        private void MainForm_KeyDown(object sender, KeyEventArgs e)
        {
            if(e.KeyCode == Keys.F5)
            {
                ExecuteNextInstruction();
            }
        }

        private void ExecuteNextInstruction()
        {
            if (cpu == null) return;
            cpu.ExecuteNext();
            UpdateScreen();
        }

        private void UpdateMemory()
        {
            if (cpu == null) return;

            var low = int.Parse(txtLowRange.Text, System.Globalization.NumberStyles.HexNumber);
            var high = int.Parse(txtHighRange.Text, System.Globalization.NumberStyles.HexNumber);

            StringBuilder sb = new StringBuilder(1000);
            for (int pc = low; pc <= high; pc += 0x10)
            {
                sb.Append($"${pc:X4}: ");
                for (int ii = 0x00; ii <= 0x07; ii++)
                {
                    sb.Append($"{cpu.SystemBus.Read((ushort)(pc + ii)):X2} ");
                }
                sb.Append(" - ");
                for (int ii = 0x08; ii <= 0x0f; ii++)
                {
                    sb.Append($"{cpu.SystemBus.Read((ushort)(pc + ii)):X2} ");
                }
                sb.AppendLine();
            }
            txtMemory.Text = sb.ToString();
        }

        private void txtLowRange_Enter(object sender, EventArgs e)
        {
            txtLowRange.SelectAll();
        }

        private void txtHighRange_Enter(object sender, EventArgs e)
        {
            txtHighRange.SelectAll();
        }
    }
}
