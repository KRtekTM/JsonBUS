namespace JsonBUS.manager
{
    public partial class ManualTest
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.leftIndicator = new System.Windows.Forms.CheckBox();
            this.rightIndicator = new System.Windows.Forms.CheckBox();
            this.highBeam = new System.Windows.Forms.CheckBox();
            this.engine = new System.Windows.Forms.CheckBox();
            this.SuspendLayout();
            // 
            // leftIndicator
            // 
            this.leftIndicator.Appearance = System.Windows.Forms.Appearance.Button;
            this.leftIndicator.AutoSize = true;
            this.leftIndicator.BackgroundImage = global::JsonBUS.manager.Properties.Resources.left;
            this.leftIndicator.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.leftIndicator.FlatAppearance.BorderSize = 0;
            this.leftIndicator.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.leftIndicator.Location = new System.Drawing.Point(230, 32);
            this.leftIndicator.MaximumSize = new System.Drawing.Size(23, 25);
            this.leftIndicator.MinimumSize = new System.Drawing.Size(23, 25);
            this.leftIndicator.Name = "leftIndicator";
            this.leftIndicator.Size = new System.Drawing.Size(23, 25);
            this.leftIndicator.TabIndex = 19;
            this.leftIndicator.UseVisualStyleBackColor = true;
            this.leftIndicator.CheckedChanged += new System.EventHandler(this.leftIndicator_CheckedChanged);
            // 
            // rightIndicator
            // 
            this.rightIndicator.Appearance = System.Windows.Forms.Appearance.Button;
            this.rightIndicator.AutoSize = true;
            this.rightIndicator.BackgroundImage = global::JsonBUS.manager.Properties.Resources.right;
            this.rightIndicator.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.rightIndicator.FlatAppearance.BorderSize = 0;
            this.rightIndicator.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.rightIndicator.Location = new System.Drawing.Point(457, 32);
            this.rightIndicator.MaximumSize = new System.Drawing.Size(23, 25);
            this.rightIndicator.MinimumSize = new System.Drawing.Size(23, 25);
            this.rightIndicator.Name = "rightIndicator";
            this.rightIndicator.Size = new System.Drawing.Size(23, 25);
            this.rightIndicator.TabIndex = 20;
            this.rightIndicator.UseVisualStyleBackColor = true;
            this.rightIndicator.CheckedChanged += new System.EventHandler(this.rightIndicator_CheckedChanged);
            // 
            // highBeam
            // 
            this.highBeam.Appearance = System.Windows.Forms.Appearance.Button;
            this.highBeam.AutoSize = true;
            this.highBeam.BackgroundImage = global::JsonBUS.manager.Properties.Resources.mainBeam;
            this.highBeam.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.highBeam.FlatAppearance.BorderSize = 0;
            this.highBeam.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.highBeam.Location = new System.Drawing.Point(344, 80);
            this.highBeam.MaximumSize = new System.Drawing.Size(21, 16);
            this.highBeam.MinimumSize = new System.Drawing.Size(21, 16);
            this.highBeam.Name = "highBeam";
            this.highBeam.Size = new System.Drawing.Size(21, 16);
            this.highBeam.TabIndex = 21;
            this.highBeam.UseVisualStyleBackColor = true;
            this.highBeam.CheckedChanged += new System.EventHandler(this.highBeam_CheckedChanged);
            // 
            // engine
            // 
            this.engine.Appearance = System.Windows.Forms.Appearance.Button;
            this.engine.AutoSize = true;
            this.engine.BackgroundImage = global::JsonBUS.manager.Properties.Resources.engine;
            this.engine.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.engine.FlatAppearance.BorderSize = 0;
            this.engine.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.engine.Location = new System.Drawing.Point(590, 138);
            this.engine.MaximumSize = new System.Drawing.Size(24, 16);
            this.engine.MinimumSize = new System.Drawing.Size(24, 16);
            this.engine.Name = "engine";
            this.engine.Size = new System.Drawing.Size(24, 16);
            this.engine.TabIndex = 22;
            this.engine.UseVisualStyleBackColor = true;
            this.engine.CheckedChanged += new System.EventHandler(this.engine_CheckedChanged);
            // 
            // ManualTest
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackgroundImage = global::JsonBUS.manager.Properties.Resources.back_bw;
            this.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.ClientSize = new System.Drawing.Size(709, 295);
            this.Controls.Add(this.engine);
            this.Controls.Add(this.highBeam);
            this.Controls.Add(this.rightIndicator);
            this.Controls.Add(this.leftIndicator);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.MaximumSize = new System.Drawing.Size(725, 334);
            this.MinimumSize = new System.Drawing.Size(725, 334);
            this.Name = "ManualTest";
            this.ShowIcon = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "ManualTest";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        public System.Windows.Forms.CheckBox leftIndicator;
        public System.Windows.Forms.CheckBox rightIndicator;
        public System.Windows.Forms.CheckBox highBeam;
        public System.Windows.Forms.CheckBox engine;
    }
}