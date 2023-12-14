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
            this.SuspendLayout();
            // 
            // scanButton
            // 
            this.scanButton.Location = new System.Drawing.Point(12, 12);
            this.scanButton.Name = "scanButton";
            this.scanButton.Size = new System.Drawing.Size(73, 21);
            this.scanButton.TabIndex = 0;
            this.scanButton.Text = "Scan";
            this.scanButton.UseVisualStyleBackColor = true;
            this.scanButton.Click += new System.EventHandler(this.ScanButton_Click);
            // 
            // ScanResult
            // 
            this.ScanResult.Location = new System.Drawing.Point(91, 12);
            this.ScanResult.Name = "ScanResult";
            this.ScanResult.Size = new System.Drawing.Size(152, 21);
            this.ScanResult.TabIndex = 2;
            this.ScanResult.Text = "No Scan performed";
            this.ScanResult.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // AudioSelector
            // 
            this.AudioSelector.FormattingEnabled = true;
            this.AudioSelector.Location = new System.Drawing.Point(12, 59);
            this.AudioSelector.Name = "AudioSelector";
            this.AudioSelector.Size = new System.Drawing.Size(354, 21);
            this.AudioSelector.TabIndex = 3;
            this.AudioSelector.DropDown += new System.EventHandler(this.AudioSelector_DropDown);
            this.AudioSelector.SelectedIndexChanged += new System.EventHandler(this.AudioSelector_SelectedIndexChanged_1);
            // 
            // progressBar1
            // 
            this.progressBar1.Location = new System.Drawing.Point(12, 39);
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(353, 10);
            this.progressBar1.TabIndex = 4;
            // 
            // muteCheckBox
            // 
            this.muteCheckBox.Checked = true;
            this.muteCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.muteCheckBox.Location = new System.Drawing.Point(305, 12);
            this.muteCheckBox.Name = "muteCheckBox";
            this.muteCheckBox.Size = new System.Drawing.Size(61, 21);
            this.muteCheckBox.TabIndex = 5;
            this.muteCheckBox.Text = "Mute";
            this.muteCheckBox.UseVisualStyleBackColor = true;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(378, 92);
            this.Controls.Add(this.muteCheckBox);
            this.Controls.Add(this.progressBar1);
            this.Controls.Add(this.AudioSelector);
            this.Controls.Add(this.ScanResult);
            this.Controls.Add(this.scanButton);
            this.Name = "Form1";
            this.Text = "Matrix Buddy Bridge";
            this.ResumeLayout(false);
        }

        private System.Windows.Forms.ProgressBar progressBar1;
        private System.Windows.Forms.CheckBox muteCheckBox;

        private System.Windows.Forms.ComboBox AudioSelector;

        private System.Windows.Forms.Label ScanResult;

        private System.Windows.Forms.Button scanButton;

        #endregion
    }
}