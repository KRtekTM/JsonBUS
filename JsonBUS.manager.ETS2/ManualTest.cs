using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace JsonBUS.manager
{
    public partial class ManualTest : Form
    {

        public ManualTest()
        {
            InitializeComponent();
        }

        private void leftIndicator_CheckedChanged(object sender, EventArgs e)
        {
            leftIndicator.BackgroundImage = (leftIndicator.Checked ? Properties.Resources.leftT : Properties.Resources.left);
        }

        private void rightIndicator_CheckedChanged(object sender, EventArgs e)
        {
            rightIndicator.BackgroundImage = (rightIndicator.Checked ? Properties.Resources.rightT : Properties.Resources.right);
        }

        private void highBeam_CheckedChanged(object sender, EventArgs e)
        {
            highBeam.BackgroundImage = (highBeam.Checked ? Properties.Resources.mainBeamT : Properties.Resources.mainBeam);
        }

        private void engine_CheckedChanged(object sender, EventArgs e)
        {
            engine.BackgroundImage = (engine.Checked ? Properties.Resources.engineT : Properties.Resources.engine);
            this.BackgroundImage = (engine.Checked ? Properties.Resources.back_bw_on : Properties.Resources.back_bw);
        }
    }
}
