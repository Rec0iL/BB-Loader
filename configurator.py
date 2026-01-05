import gradio as gr
import serial
import serial.tools.list_ports
import time

# Global Serial Object
ser = None

def get_ports():
    ports = [p.device for p in serial.tools.list_ports.comports()]
    return ports if ports else ["No Ports Found"]

def connect(port):
    global ser
    if not port or port == "No Ports Found":
        return "Please select a valid port"
    try:
        ser = serial.Serial(port, 115200, timeout=0.1)
        return f"Connected to {port}"
    except Exception as e:
        return f"Error: {e}"

def save_settings(sbb, speed, ramp, rev):
    if ser:
        ser.write(f"SET_SBB:{sbb}\n".encode())
        ser.write(f"SET_SPEED:{speed}\n".encode())
        ser.write(f"SET_RAMP:{ramp}\n".encode())
        ser.write(f"SET_DIR:{1 if rev else 0}\n".encode())
        return "Settings Saved to Hardware!"
    return "Not Connected"

def start_loading(amount, progress=gr.Progress()):
    if not ser:
        return "Not Connected"

    ser.write(f"START:{amount}\n".encode())
    progress(0, desc="Starting...")

    remaining = amount
    while remaining > 0:
        line = ser.readline().decode('utf-8', errors='ignore').strip()
        if line.startswith("PROGRESS:"):
            try:
                remaining = int(line.split(":")[1])
                prog_value = (amount - remaining) / amount
                progress(prog_value, desc=f"Loading... {remaining} BBs left")
            except:
                pass
        elif "FINISHED" in line or "STOPPED" in line:
            break

    return "Finished / Stopped"

def stop_loading():
    if ser:
        ser.write(b"S\n")
    return "Stop Signal Sent"

# --- GRADIO UI ---
with gr.Blocks(title="BB Loader Pro") as demo:
    gr.Markdown("# 🎯 Airsoft BB Loader Configurator")

    with gr.Row():
        with gr.Column(scale=1):
            gr.Markdown("### 🔌 Connection")
            port_dropdown = gr.Dropdown(choices=get_ports(), label="Select Port")
            conn_btn = gr.Button("Connect")
            status_label = gr.Label("Disconnected")

            gr.Markdown("### ⚙️ Settings")
            sbb_input = gr.Number(value=100, label="Steps per BB")
            speed_slider = gr.Slider(200, 2000, value=800, label="Step Delay (Speed)")
            ramp_slider = gr.Slider(0, 2000, value=300, label="Ramp Steps")
            rev_check = gr.Checkbox(label="Reverse Stepper")
            save_btn = gr.Button("Save to EEPROM")

        with gr.Column(scale=2):
            gr.Markdown("### 🎮 Operation")

            # Mag Presets
            gr.Markdown("#### Quick Presets (Magazine Size)")
            with gr.Row():
                btn_150 = gr.Button("150")
                btn_120 = gr.Button("120")
                btn_100 = gr.Button("100")
                btn_60 = gr.Button("60")
                btn_30 = gr.Button("30")
                btn_25 = gr.Button("25")

            bb_amount = gr.Number(value=120, label="Target BB Count")

            # Manual Adjustment Grid
            with gr.Row():
                with gr.Column(min_width=50):
                    btn_p1 = gr.Button("+1")
                    btn_m1 = gr.Button("-1")
                with gr.Column(min_width=50):
                    btn_p5 = gr.Button("+5")
                    btn_m5 = gr.Button("-5")
                with gr.Column(min_width=50):
                    btn_p10 = gr.Button("+10")
                    btn_m10 = gr.Button("-10")

            btn_clr = gr.Button("Reset to 0", variant="secondary")

            gr.Markdown("---")
            start_btn = gr.Button("🚀 START LOADING", variant="primary")
            stop_btn = gr.Button("🛑 EMERGENCY STOP", variant="stop")

            progress_text = gr.Textbox(label="Last Status")

    # --- LOGIC ---
    conn_btn.click(connect, inputs=port_dropdown, outputs=status_label)
    save_btn.click(save_settings, inputs=[sbb_input, speed_slider, ramp_slider, rev_check], outputs=status_label)

    # Preset Logic
    btn_150.click(lambda: 150, outputs=bb_amount)
    btn_120.click(lambda: 120, outputs=bb_amount)
    btn_100.click(lambda: 100, outputs=bb_amount)
    btn_60.click(lambda: 60, outputs=bb_amount)
    btn_30.click(lambda: 30, outputs=bb_amount)
    btn_25.click(lambda: 25, outputs=bb_amount)

    # Counter increments/decrements
    btn_p1.click(lambda x: x + 1, inputs=bb_amount, outputs=bb_amount)
    btn_m1.click(lambda x: max(0, x - 1), inputs=bb_amount, outputs=bb_amount)
    btn_p5.click(lambda x: x + 5, inputs=bb_amount, outputs=bb_amount)
    btn_m5.click(lambda x: max(0, x - 5), inputs=bb_amount, outputs=bb_amount)
    btn_p10.click(lambda x: x + 10, inputs=bb_amount, outputs=bb_amount)
    btn_m10.click(lambda x: max(0, x - 10), inputs=bb_amount, outputs=bb_amount)
    btn_clr.click(lambda: 0, outputs=bb_amount)

    start_btn.click(start_loading, inputs=bb_amount, outputs=progress_text)
    stop_btn.click(stop_loading, outputs=progress_text)

if __name__ == "__main__":
    demo.launch()
