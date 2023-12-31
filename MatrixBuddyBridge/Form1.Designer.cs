namespace MatrixBuddyBridge
{
    partial class Form1
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
            this.scanButton = new System.Windows.Forms.Button();
            this.ScanResult = new System.Windows.Forms.Label();
            this.AudioSelector = new System.Windows.Forms.ComboBox();
            this.progressBar1 = new System.Windows.Forms.ProgressBar();
            this.muteCheckBox = new System.Windows.Forms.CheckBox();
            this.panel1 = new System.Windows.Forms.Panel();
            this.trackBar1 = new System.Windows.Forms.TrackBar();
            this.label1 = new System.Windows.Forms.Label();
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar1)).BeginInit();
            this.SuspendLayout();
            // 
            // scanButton
            // 
            this.scanButton.Location = new System.Drawing.Point(18, 18);
            this.scanButton.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.scanButton.Name = "scanButton";
            this.scanButton.Size = new System.Drawing.Size(110, 32);
            this.scanButton.TabIndex = 0;
            this.scanButton.Text = "Scan";
            this.scanButton.UseVisualStyleBackColor = true;
            this.scanButton.Click += new System.EventHandler(this.ScanButton_Click);
            // 
            // ScanResult
            // 
            this.ScanResult.Location = new System.Drawing.Point(136, 18);
            this.ScanResult.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.ScanResult.Name = "ScanResult";
            this.ScanResult.Size = new System.Drawing.Size(228, 32);
            this.ScanResult.TabIndex = 2;
            this.ScanResult.Text = "No Scan performed";
            this.ScanResult.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // AudioSelector
            // 
            this.AudioSelector.FormattingEnabled = true;
            this.AudioSelector.Location = new System.Drawing.Point(18, 136);
            this.AudioSelector.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.AudioSelector.Name = "AudioSelector";
            this.AudioSelector.Size = new System.Drawing.Size(529, 28);
            this.AudioSelector.TabIndex = 3;
            this.AudioSelector.DropDown += new System.EventHandler(this.AudioSelector_DropDown);
            this.AudioSelector.SelectedIndexChanged += new System.EventHandler(this.AudioSelector_SelectedIndexChanged);
            // 
            // progressBar1
            // 
            this.progressBar1.Location = new System.Drawing.Point(17, 110);
            this.progressBar1.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.progressBar1.MarqueeAnimationSpeed = 10000;
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(530, 15);
            this.progressBar1.TabIndex = 4;
            // 
            // muteCheckBox
            // 
            this.muteCheckBox.Checked = true;
            this.muteCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.muteCheckBox.Location = new System.Drawing.Point(458, 18);
            this.muteCheckBox.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.muteCheckBox.Name = "muteCheckBox";
            this.muteCheckBox.Size = new System.Drawing.Size(92, 32);
            this.muteCheckBox.TabIndex = 5;
            this.muteCheckBox.Text = "Mute";
            this.muteCheckBox.UseVisualStyleBackColor = true;
            // 
            // panel1
            // 
            this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel1.Controls.Add(this.trackBar1);
            this.panel1.Location = new System.Drawing.Point(17, 58);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(530, 44);
            this.panel1.TabIndex = 7;
            // 
            // trackBar1
            // 
            this.trackBar1.Location = new System.Drawing.Point(-5, 12);
            this.trackBar1.Maximum = 100;
            this.trackBar1.Name = "trackBar1";
            this.trackBar1.Size = new System.Drawing.Size(538, 45);
            this.trackBar1.TabIndex = 0;
            this.trackBar1.TickStyle = System.Windows.Forms.TickStyle.None;
            this.trackBar1.Value = 50;
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(21, 50);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(163, 24);
            this.label1.TabIndex = 1;
            this.label1.Text = "Talk/Sing Threshold";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(567, 182);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.muteCheckBox);
            this.Controls.Add(this.progressBar1);
            this.Controls.Add(this.AudioSelector);
            this.Controls.Add(this.ScanResult);
            this.Controls.Add(this.scanButton);
            this.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.Name = "Form1";
            this.Text = "Matrix Buddy Bridge";
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar1)).EndInit();
            this.ResumeLayout(false);
        }

        private System.Windows.Forms.Label label1;

        private System.Windows.Forms.Panel panel1;

        private System.Windows.Forms.TrackBar trackBar1;

        private System.Windows.Forms.ProgressBar progressBar1;
        private System.Windows.Forms.CheckBox muteCheckBox;

        private System.Windows.Forms.ComboBox AudioSelector;

        private System.Windows.Forms.Label ScanResult;

        private System.Windows.Forms.Button scanButton;

        #endregion
    }
}