namespace JsonBUS.manager
{
    partial class ManagerForm
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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ManagerForm));
            this.TaskbarIcon = new System.Windows.Forms.NotifyIcon(this.components);
            this.button_ets2 = new System.Windows.Forms.Button();
            this.panel1 = new System.Windows.Forms.Panel();
            this.button1 = new System.Windows.Forms.Button();
            this.button_manualtest = new System.Windows.Forms.Button();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // TaskbarIcon
            // 
            this.TaskbarIcon.Icon = ((System.Drawing.Icon)(resources.GetObject("TaskbarIcon.Icon")));
            this.TaskbarIcon.Text = "JsonBUS - disconnected";
            this.TaskbarIcon.Visible = true;
            // 
            // button_ets2
            // 
            this.button_ets2.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.button_ets2.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("button_ets2.BackgroundImage")));
            this.button_ets2.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
            this.button_ets2.Location = new System.Drawing.Point(7, 7);
            this.button_ets2.Name = "button_ets2";
            this.button_ets2.Size = new System.Drawing.Size(270, 150);
            this.button_ets2.TabIndex = 0;
            this.button_ets2.UseVisualStyleBackColor = true;
            this.button_ets2.Click += new System.EventHandler(this.button_ets2_Click);
            // 
            // panel1
            // 
            this.panel1.AutoScroll = true;
            this.panel1.Controls.Add(this.button1);
            this.panel1.Controls.Add(this.button_manualtest);
            this.panel1.Controls.Add(this.button_ets2);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(284, 361);
            this.panel1.TabIndex = 1;
            // 
            // button1
            // 
            this.button1.Image = ((System.Drawing.Image)(resources.GetObject("button1.Image")));
            this.button1.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.button1.Location = new System.Drawing.Point(7, 324);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(270, 30);
            this.button1.TabIndex = 2;
            this.button1.Text = "Open eBolero app in web browser";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // button_manualtest
            // 
            this.button_manualtest.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.button_manualtest.Location = new System.Drawing.Point(7, 288);
            this.button_manualtest.Name = "button_manualtest";
            this.button_manualtest.Size = new System.Drawing.Size(270, 30);
            this.button_manualtest.TabIndex = 1;
            this.button_manualtest.Text = "Manual test of Instrument Cluster";
            this.button_manualtest.UseVisualStyleBackColor = true;
            this.button_manualtest.Click += new System.EventHandler(this.button_manualtest_Click);
            // 
            // ManagerForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(284, 361);
            this.Controls.Add(this.panel1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximumSize = new System.Drawing.Size(300, 400);
            this.MinimumSize = new System.Drawing.Size(300, 210);
            this.Name = "ManagerForm";
            this.Text = "JsonBUS manager";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.panel1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.NotifyIcon TaskbarIcon;
        private System.Windows.Forms.Button button_ets2;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button button_manualtest;
        private System.Windows.Forms.Button button1;
    }
}

