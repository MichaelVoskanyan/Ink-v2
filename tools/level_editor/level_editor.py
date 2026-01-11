#!/usr/bin/env python3
import json
import os
import shutil
import tkinter as tk
from tkinter import filedialog, messagebox, simpledialog
from tkinter import ttk

try:
    from PIL import Image, ImageTk
except ImportError:
    Image = None
    ImageTk = None


class LevelEditor:
    def __init__(self, root):
        self.root = root
        self.root.title("Ink Level Editor")

        self.repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
        self.levels_dir = os.path.join(self.repo_root, "assets", "levels")
        self.textures_dir = os.path.join(self.repo_root, "assets", "textures")

        self.textures = self._load_textures()
        self.texture_sources = {}
        self.texture_images = self._load_texture_images()
        self.texture_cache = {}
        self.objects = []
        self.current_file = None
        self.selected_index = None
        self.canvas_items = {}
        self.dragging_index = None
        self.drag_mode = None
        self.drag_start = None
        self.panning = False
        self.pan_start = None

        self._build_ui()
        self._new_level()

    def _load_textures(self):
        textures = []
        if os.path.isdir(self.textures_dir):
            for name in sorted(os.listdir(self.textures_dir)):
                if name.lower().endswith(".png"):
                    textures.append(os.path.splitext(name)[0])
        return textures or ["default_brick"]

    def _load_texture_images(self):
        images = {}
        for name in self.textures:
            path = os.path.join(self.textures_dir, f"{name}.png")
            if not os.path.isfile(path):
                continue
            if Image and ImageTk:
                try:
                    pil_img = Image.open(path)
                    max_size = 64
                    scale = max(pil_img.width / max_size, pil_img.height / max_size)
                    if scale > 1:
                        size = (int(pil_img.width / scale), int(pil_img.height / scale))
                        pil_img = pil_img.resize(size, Image.LANCZOS)
                    images[name] = ImageTk.PhotoImage(pil_img)
                except (OSError, ValueError):
                    continue
            else:
                try:
                    full_image = tk.PhotoImage(file=path)
                except tk.TclError:
                    continue
                self.texture_sources[name] = full_image
                max_size = 64
                scale = max(full_image.width() / max_size, full_image.height() / max_size)
                if scale > 1:
                    factor = int(scale) + 1
                    full_image = full_image.subsample(factor, factor)
                images[name] = full_image
        return images

    def _build_ui(self):
        self._build_menu()

        top_frame = ttk.Frame(self.root, padding=10)
        top_frame.pack(side=tk.TOP, fill=tk.X)

        ttk.Label(top_frame, text="Level Name").pack(side=tk.LEFT)
        self.level_name_var = tk.StringVar()
        ttk.Entry(top_frame, textvariable=self.level_name_var, width=40).pack(
            side=tk.LEFT, padx=8
        )

        main_frame = ttk.Frame(self.root, padding=10)
        main_frame.pack(side=tk.TOP, fill=tk.BOTH, expand=True)

        left_frame = ttk.Frame(main_frame)
        left_frame.pack(side=tk.LEFT, fill=tk.Y)

        self.object_list = tk.Listbox(left_frame, height=18, width=30)
        self.object_list.pack(side=tk.TOP, fill=tk.Y, expand=True)
        self.object_list.bind("<<ListboxSelect>>", self._on_select_object)

        button_frame = ttk.Frame(left_frame)
        button_frame.pack(side=tk.TOP, fill=tk.X, pady=8)

        ttk.Button(button_frame, text="Add Platform", command=self._add_platform).pack(
            side=tk.TOP, fill=tk.X, pady=2
        )
        ttk.Button(button_frame, text="Add Character", command=self._add_character).pack(
            side=tk.TOP, fill=tk.X, pady=2
        )
        ttk.Button(button_frame, text="Add Entity...", command=self._add_custom_entity).pack(
            side=tk.TOP, fill=tk.X, pady=2
        )
        ttk.Button(button_frame, text="Delete Selected", command=self._delete_selected).pack(
            side=tk.TOP, fill=tk.X, pady=2
        )

        right_frame = ttk.Frame(main_frame)
        right_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=(12, 0))

        view_frame = ttk.Frame(right_frame)
        view_frame.pack(side=tk.TOP, fill=tk.X)

        self.view_vars = {
            "ppu": tk.StringVar(value="60"),
            "origin_x": tk.StringVar(value="0"),
            "origin_y": tk.StringVar(value="0"),
            "snap": tk.BooleanVar(value=False),
            "snap_size": tk.StringVar(value="0.5"),
        }

        ttk.Label(view_frame, text="Pixels/Unit").pack(side=tk.LEFT)
        ttk.Entry(view_frame, textvariable=self.view_vars["ppu"], width=6).pack(
            side=tk.LEFT, padx=(4, 8)
        )
        ttk.Label(view_frame, text="Origin (x,y)").pack(side=tk.LEFT)
        ttk.Entry(view_frame, textvariable=self.view_vars["origin_x"], width=6).pack(
            side=tk.LEFT, padx=(4, 2)
        )
        ttk.Entry(view_frame, textvariable=self.view_vars["origin_y"], width=6).pack(
            side=tk.LEFT, padx=(2, 8)
        )
        ttk.Checkbutton(
            view_frame, text="Snap", variable=self.view_vars["snap"]
        ).pack(side=tk.LEFT)
        ttk.Entry(view_frame, textvariable=self.view_vars["snap_size"], width=5).pack(
            side=tk.LEFT, padx=(4, 8)
        )
        ttk.Button(view_frame, text="Apply View", command=self._redraw_canvas).pack(
            side=tk.LEFT
        )

        self.canvas = tk.Canvas(right_frame, width=700, height=400, bg="#1b1e23")
        self.canvas.pack(side=tk.TOP, fill=tk.BOTH, expand=True, pady=(8, 12))
        self.canvas.bind("<Configure>", lambda event: self._redraw_canvas())
        self.canvas.bind("<ButtonPress-1>", self._on_canvas_press)
        self.canvas.bind("<B1-Motion>", self._on_canvas_drag)
        self.canvas.bind("<ButtonRelease-1>", self._on_canvas_release)
        self.canvas.bind("<ButtonPress-2>", self._on_pan_press)
        self.canvas.bind("<B2-Motion>", self._on_pan_drag)
        self.canvas.bind("<ButtonRelease-2>", self._on_pan_release)
        self.canvas.bind("<ButtonPress-3>", self._on_pan_press)
        self.canvas.bind("<B3-Motion>", self._on_pan_drag)
        self.canvas.bind("<ButtonRelease-3>", self._on_pan_release)

        self.texture_palette = ttk.Frame(right_frame)
        self.texture_palette.pack(side=tk.TOP, fill=tk.X, pady=(0, 10))
        ttk.Label(self.texture_palette, text="Textures").pack(side=tk.LEFT, padx=(0, 8))
        self._build_texture_palette()

        self.form_vars = {
            "type": tk.StringVar(),
            "subtype": tk.StringVar(),
            "texture": tk.StringVar(),
            "pos_x": tk.StringVar(),
            "pos_y": tk.StringVar(),
            "pos_z": tk.StringVar(),
            "scale_x": tk.StringVar(),
            "scale_y": tk.StringVar(),
            "speed": tk.StringVar(),
            "mass": tk.StringVar(),
            "move_speed": tk.StringVar(),
            "path_ax": tk.StringVar(),
            "path_ay": tk.StringVar(),
            "path_bx": tk.StringVar(),
            "path_by": tk.StringVar(),
        }

        form = ttk.Frame(right_frame)
        form.pack(side=tk.TOP, fill=tk.BOTH, expand=True)

        row = 0
        ttk.Label(form, text="Type").grid(row=row, column=0, sticky="w")
        self.type_entry = ttk.Entry(form, textvariable=self.form_vars["type"], width=24)
        self.type_entry.grid(row=row, column=1, sticky="w", pady=2)
        self.type_entry.configure(state="readonly")

        row += 1
        ttk.Label(form, text="Subtype").grid(row=row, column=0, sticky="w")
        self.subtype_box = ttk.Combobox(
            form, textvariable=self.form_vars["subtype"], values=["stationary", "moving"], width=21
        )
        self.subtype_box.grid(row=row, column=1, sticky="w", pady=2)
        self.subtype_box.bind("<<ComboboxSelected>>", lambda event: self._update_form_state())

        row += 1
        ttk.Label(form, text="Texture").grid(row=row, column=0, sticky="w")
        self.texture_box = ttk.Combobox(
            form, textvariable=self.form_vars["texture"], values=self.textures, width=21
        )
        self.texture_box.grid(row=row, column=1, sticky="w", pady=2)

        row += 1
        ttk.Label(form, text="Position (x,y,z)").grid(row=row, column=0, sticky="w")
        pos_frame = ttk.Frame(form)
        pos_frame.grid(row=row, column=1, sticky="w", pady=2)
        ttk.Entry(pos_frame, textvariable=self.form_vars["pos_x"], width=6).pack(
            side=tk.LEFT
        )
        ttk.Entry(pos_frame, textvariable=self.form_vars["pos_y"], width=6).pack(
            side=tk.LEFT, padx=4
        )
        ttk.Entry(pos_frame, textvariable=self.form_vars["pos_z"], width=6).pack(
            side=tk.LEFT
        )

        row += 1
        ttk.Label(form, text="Scale (x,y)").grid(row=row, column=0, sticky="w")
        scale_frame = ttk.Frame(form)
        scale_frame.grid(row=row, column=1, sticky="w", pady=2)
        ttk.Entry(scale_frame, textvariable=self.form_vars["scale_x"], width=6).pack(
            side=tk.LEFT
        )
        ttk.Entry(scale_frame, textvariable=self.form_vars["scale_y"], width=6).pack(
            side=tk.LEFT, padx=4
        )

        row += 1
        ttk.Label(form, text="Speed").grid(row=row, column=0, sticky="w")
        self.speed_entry = ttk.Entry(form, textvariable=self.form_vars["speed"], width=24)
        self.speed_entry.grid(row=row, column=1, sticky="w", pady=2)

        row += 1
        ttk.Label(form, text="Mass").grid(row=row, column=0, sticky="w")
        self.mass_entry = ttk.Entry(form, textvariable=self.form_vars["mass"], width=24)
        self.mass_entry.grid(row=row, column=1, sticky="w", pady=2)

        row += 1
        ttk.Label(form, text="Move Speed").grid(row=row, column=0, sticky="w")
        self.move_speed_entry = ttk.Entry(
            form, textvariable=self.form_vars["move_speed"], width=24
        )
        self.move_speed_entry.grid(row=row, column=1, sticky="w", pady=2)

        row += 1
        ttk.Label(form, text="Path A (x,y)").grid(row=row, column=0, sticky="w")
        path_a = ttk.Frame(form)
        path_a.grid(row=row, column=1, sticky="w", pady=2)
        self.path_ax_entry = ttk.Entry(path_a, textvariable=self.form_vars["path_ax"], width=6)
        self.path_ax_entry.pack(side=tk.LEFT)
        self.path_ay_entry = ttk.Entry(path_a, textvariable=self.form_vars["path_ay"], width=6)
        self.path_ay_entry.pack(side=tk.LEFT, padx=4)

        row += 1
        ttk.Label(form, text="Path B (x,y)").grid(row=row, column=0, sticky="w")
        path_b = ttk.Frame(form)
        path_b.grid(row=row, column=1, sticky="w", pady=2)
        self.path_bx_entry = ttk.Entry(path_b, textvariable=self.form_vars["path_bx"], width=6)
        self.path_bx_entry.pack(side=tk.LEFT)
        self.path_by_entry = ttk.Entry(path_b, textvariable=self.form_vars["path_by"], width=6)
        self.path_by_entry.pack(side=tk.LEFT, padx=4)

        row += 1
        ttk.Button(form, text="Apply Changes", command=self._apply_changes).grid(
            row=row, column=0, columnspan=2, sticky="w", pady=(8, 2)
        )

        self._update_form_state()

    def _build_menu(self):
        menu = tk.Menu(self.root)
        file_menu = tk.Menu(menu, tearoff=0)
        file_menu.add_command(label="New", command=self._new_level)
        file_menu.add_command(label="Open...", command=self._open_level)
        file_menu.add_command(label="Save", command=self._save_level)
        file_menu.add_command(label="Save As...", command=self._save_level_as)
        file_menu.add_separator()
        file_menu.add_command(label="Exit", command=self.root.quit)
        menu.add_cascade(label="File", menu=file_menu)
        self.root.config(menu=menu)

    def _new_level(self):
        self.level_name_var.set("New Level")
        self.objects = []
        self.current_file = None
        self._refresh_list()
        self._clear_form()
        self._redraw_canvas()

    def _open_level(self):
        filename = filedialog.askopenfilename(
            title="Open Level",
            initialdir=self.levels_dir,
            filetypes=[("JSON files", "*.json")],
        )
        if not filename:
            return
        try:
            with open(filename, "r", encoding="utf-8") as handle:
                data = json.load(handle)
        except (OSError, json.JSONDecodeError) as exc:
            messagebox.showerror("Open Failed", f"Could not open file:\n{exc}")
            return
        self.level_name_var.set(data.get("levelName", "Unnamed Level"))
        self.objects = data.get("objects", [])
        self.current_file = filename
        self._refresh_list()
        self._clear_form()
        self._redraw_canvas()

    def _save_level(self):
        if not self.current_file:
            return self._save_level_as()
        return self._write_file(self.current_file)

    def _save_level_as(self):
        filename = filedialog.asksaveasfilename(
            title="Save Level As",
            initialdir=self.levels_dir,
            defaultextension=".json",
            filetypes=[("JSON files", "*.json")],
        )
        if not filename:
            return False
        self.current_file = filename
        return self._write_file(filename)

    def _write_file(self, filename):
        if self._count_characters() > 1:
            messagebox.showwarning(
                "Multiple Characters",
                "Only one character is supported by the loader. Consider removing extras.",
            )
        data = {"levelName": self.level_name_var.get().strip() or "Unnamed Level"}
        data["objects"] = self.objects
        try:
            with open(filename, "w", encoding="utf-8") as handle:
                json.dump(data, handle, indent=2)
        except OSError as exc:
            messagebox.showerror("Save Failed", f"Could not save file:\n{exc}")
            return False
        return True

    def _refresh_list(self):
        self.object_list.delete(0, tk.END)
        for idx, obj in enumerate(self.objects):
            label = self._object_label(obj, idx)
            self.object_list.insert(tk.END, label)
        self._redraw_canvas()

    def _object_label(self, obj, idx):
        obj_type = obj.get("type", "unknown")
        subtype = obj.get("subtype")
        if subtype:
            return f"{idx}: {obj_type} ({subtype})"
        return f"{idx}: {obj_type}"

    def _add_platform(self):
        obj = {
            "type": "platform",
            "subtype": "stationary",
            "texture": self.textures[0],
            "position": [0.0, 0.0, 0.0],
            "scale": [1.0, 1.0],
            "move_speed": 1.0,
            "path": [[0.0, 0.0], [1.0, 0.0]],
        }
        self.objects.append(obj)
        self._refresh_list()

    def _add_character(self):
        if self._count_characters() >= 1:
            messagebox.showinfo("Character Exists", "Only one character is supported.")
            return
        obj = {
            "type": "character",
            "texture": self.textures[0],
            "position": [0.0, 0.0, 0.0],
            "scale": [0.2, 0.2],
            "speed": 2.5,
            "mass": 0.2,
        }
        self.objects.append(obj)
        self._refresh_list()

    def _add_custom_entity(self):
        entity_type = simpledialog.askstring("Entity Type", "Enter custom entity type:")
        if not entity_type:
            return
        obj = {
            "type": entity_type.strip(),
            "texture": self.textures[0],
            "position": [0.0, 0.0, 0.0],
            "scale": [1.0, 1.0],
        }
        self.objects.append(obj)
        self._refresh_list()

    def _delete_selected(self):
        idx = self._current_index()
        if idx is None:
            return
        del self.objects[idx]
        self._refresh_list()
        self._clear_form()
        self._redraw_canvas()

    def _current_index(self):
        selection = self.object_list.curselection()
        if not selection:
            return None
        return selection[0]

    def _on_select_object(self, event):
        idx = self._current_index()
        if idx is None:
            return
        self.selected_index = idx
        self._load_form(self.objects[idx])
        self._redraw_canvas()

    def _clear_form(self):
        for key in self.form_vars:
            self.form_vars[key].set("")
        self.selected_index = None
        self._update_form_state()
        self._redraw_canvas()

    def _load_form(self, obj):
        self.form_vars["type"].set(obj.get("type", ""))
        self.form_vars["subtype"].set(obj.get("subtype", "stationary"))
        self.form_vars["texture"].set(obj.get("texture", self.textures[0]))
        position = obj.get("position", [0.0, 0.0, 0.0])
        scale = obj.get("scale", [1.0, 1.0])
        self.form_vars["pos_x"].set(str(position[0]))
        self.form_vars["pos_y"].set(str(position[1]))
        self.form_vars["pos_z"].set(str(position[2]))
        self.form_vars["scale_x"].set(str(scale[0]))
        self.form_vars["scale_y"].set(str(scale[1]))
        self.form_vars["speed"].set(str(obj.get("speed", 2.5)))
        self.form_vars["mass"].set(str(obj.get("mass", 0.2)))
        self.form_vars["move_speed"].set(str(obj.get("move_speed", 1.0)))
        path = obj.get("path", [[0.0, 0.0], [1.0, 0.0]])
        if len(path) < 2:
            path = [[0.0, 0.0], [1.0, 0.0]]
        self.form_vars["path_ax"].set(str(path[0][0]))
        self.form_vars["path_ay"].set(str(path[0][1]))
        self.form_vars["path_bx"].set(str(path[1][0]))
        self.form_vars["path_by"].set(str(path[1][1]))
        self._update_form_state()

    def _update_form_state(self):
        obj_type = self.form_vars["type"].get()
        is_platform = obj_type == "platform"
        is_character = obj_type == "character"
        is_moving = is_platform and self.form_vars["subtype"].get() == "moving"

        if not obj_type:
            self.subtype_box.configure(state="disabled")
            self.speed_entry.configure(state="disabled")
            self.mass_entry.configure(state="disabled")
            self.move_speed_entry.configure(state="disabled")
            self.path_ax_entry.configure(state="disabled")
            self.path_ay_entry.configure(state="disabled")
            self.path_bx_entry.configure(state="disabled")
            self.path_by_entry.configure(state="disabled")
            return

        self.subtype_box.configure(state="readonly" if is_platform else "disabled")
        self.speed_entry.configure(state="normal" if is_character else "disabled")
        self.mass_entry.configure(state="normal" if is_character else "disabled")
        self.move_speed_entry.configure(state="normal" if is_moving else "disabled")
        state = "normal" if is_moving else "disabled"
        self.path_ax_entry.configure(state=state)
        self.path_ay_entry.configure(state=state)
        self.path_bx_entry.configure(state=state)
        self.path_by_entry.configure(state=state)

    def _apply_changes(self):
        idx = self.selected_index
        if idx is None:
            return
        obj = self.objects[idx]
        try:
            pos = [
                float(self.form_vars["pos_x"].get()),
                float(self.form_vars["pos_y"].get()),
                float(self.form_vars["pos_z"].get()),
            ]
            scale = [
                float(self.form_vars["scale_x"].get()),
                float(self.form_vars["scale_y"].get()),
            ]
        except ValueError:
            messagebox.showerror("Invalid Input", "Position and scale must be numbers.")
            return

        obj["texture"] = self.form_vars["texture"].get().strip() or self.textures[0]
        obj["position"] = pos
        obj["scale"] = scale

        obj_type = obj.get("type")
        if obj_type == "platform":
            obj["subtype"] = self.form_vars["subtype"].get().strip() or "stationary"
            if obj["subtype"] == "moving":
                try:
                    obj["move_speed"] = float(self.form_vars["move_speed"].get())
                    ax = float(self.form_vars["path_ax"].get())
                    ay = float(self.form_vars["path_ay"].get())
                    bx = float(self.form_vars["path_bx"].get())
                    by = float(self.form_vars["path_by"].get())
                except ValueError:
                    messagebox.showerror("Invalid Input", "Move speed and path must be numbers.")
                    return
                obj["path"] = [[ax, ay], [bx, by]]
        if obj_type == "character":
            try:
                obj["speed"] = float(self.form_vars["speed"].get())
                obj["mass"] = float(self.form_vars["mass"].get())
            except ValueError:
                messagebox.showerror("Invalid Input", "Speed and mass must be numbers.")
                return

        self.objects[idx] = obj
        self._refresh_list()
        self.object_list.selection_set(idx)
        self._redraw_canvas()

    def _count_characters(self):
        return sum(1 for obj in self.objects if obj.get("type") == "character")

    def _get_view_settings(self):
        try:
            pixels_per_unit = float(self.view_vars["ppu"].get())
        except ValueError:
            pixels_per_unit = 60.0
        try:
            origin_x = float(self.view_vars["origin_x"].get())
            origin_y = float(self.view_vars["origin_y"].get())
        except ValueError:
            origin_x = 0.0
            origin_y = 0.0
        if pixels_per_unit <= 0:
            pixels_per_unit = 60.0
        try:
            snap_size = float(self.view_vars["snap_size"].get())
        except ValueError:
            snap_size = 0.5
        if snap_size <= 0:
            snap_size = 0.5
        return pixels_per_unit, origin_x, origin_y, snap_size

    def _world_to_canvas(self, x, y):
        ppu, origin_x, origin_y, _ = self._get_view_settings()
        width = self.canvas.winfo_width()
        height = self.canvas.winfo_height()
        cx = width / 2
        cy = height / 2
        screen_x = cx + (x - origin_x) * ppu
        screen_y = cy - (y - origin_y) * ppu
        return screen_x, screen_y

    def _canvas_to_world(self, x, y):
        ppu, origin_x, origin_y, _ = self._get_view_settings()
        width = self.canvas.winfo_width()
        height = self.canvas.winfo_height()
        cx = width / 2
        cy = height / 2
        world_x = (x - cx) / ppu + origin_x
        world_y = (cy - y) / ppu + origin_y
        return world_x, world_y

    def _redraw_canvas(self):
        if not hasattr(self, "canvas"):
            return
        self.canvas.delete("all")
        self.canvas_items = {}

        ppu, origin_x, origin_y, snap_size = self._get_view_settings()
        width = self.canvas.winfo_width()
        height = self.canvas.winfo_height()
        if width <= 1 or height <= 1:
            return

        world_left = origin_x - (width / 2) / ppu
        world_right = origin_x + (width / 2) / ppu
        world_bottom = origin_y - (height / 2) / ppu
        world_top = origin_y + (height / 2) / ppu

        grid_color = "#2a2f36"
        axis_color = "#3a4250"

        grid_step = snap_size if self.view_vars["snap"].get() else 1.0
        if grid_step <= 0:
            grid_step = 1.0
        start_x = int(world_left / grid_step) - 1
        end_x = int(world_right / grid_step) + 1
        for gx in range(start_x, end_x + 1):
            x = gx * grid_step
            sx1, sy1 = self._world_to_canvas(x, world_bottom)
            sx2, sy2 = self._world_to_canvas(x, world_top)
            color = axis_color if x == 0 else grid_color
            self.canvas.create_line(sx1, sy1, sx2, sy2, fill=color)

        start_y = int(world_bottom / grid_step) - 1
        end_y = int(world_top / grid_step) + 1
        for gy in range(start_y, end_y + 1):
            y = gy * grid_step
            sx1, sy1 = self._world_to_canvas(world_left, y)
            sx2, sy2 = self._world_to_canvas(world_right, y)
            color = axis_color if y == 0 else grid_color
            self.canvas.create_line(sx1, sy1, sx2, sy2, fill=color)

        for idx, obj in enumerate(self.objects):
            obj_type = obj.get("type", "unknown")
            position = obj.get("position", [0.0, 0.0, 0.0])
            scale = obj.get("scale", [1.0, 1.0])
            try:
                pos_x = float(position[0])
                pos_y = float(position[1])
                scale_x = float(scale[0])
                scale_y = float(scale[1])
            except (ValueError, TypeError, IndexError):
                continue

            cx, cy = self._world_to_canvas(pos_x, pos_y)
            half_w = (scale_x * ppu) / 2
            half_h = (scale_y * ppu) / 2

            if obj_type == "platform":
                outline = "#7fd4c1"
                fill = "#0e2e2a"
            elif obj_type == "character":
                outline = "#ffb24a"
                fill = "#3b2b1a"
            else:
                outline = "#9aa4b2"
                fill = "#1e2229"

            width = 3 if idx == self.selected_index else 2
            rect_id = self.canvas.create_rectangle(
                cx - half_w,
                cy - half_h,
                cx + half_w,
                cy + half_h,
                outline=outline,
                fill=fill,
                width=width,
                tags=("object", f"obj_{idx}"),
            )

            texture_name = obj.get("texture")
            image = self._get_texture_for_size(texture_name, half_w * 2, half_h * 2)
            image_id = None
            if image:
                image_id = self.canvas.create_image(
                    cx,
                    cy,
                    image=image,
                    tags=("object", f"obj_{idx}"),
                )
            self.canvas_items[idx] = {"rect": rect_id, "image": image_id}

            if obj_type == "platform" and obj.get("subtype") == "moving":
                path = obj.get("path", [])
                if len(path) >= 2:
                    ax, ay = path[0][0], path[0][1]
                    bx, by = path[1][0], path[1][1]
                    axc, ayc = self._world_to_canvas(ax, ay)
                    bxc, byc = self._world_to_canvas(bx, by)
                    self.canvas.create_line(
                        axc,
                        ayc,
                        bxc,
                        byc,
                        fill="#4aa3ff",
                        width=2,
                        dash=(4, 2),
                    )

    def _find_object_index_from_item(self, item_id):
        tags = self.canvas.gettags(item_id)
        for tag in tags:
            if tag.startswith("obj_"):
                try:
                    return int(tag.split("_")[1])
                except ValueError:
                    return None
        return None

    def _on_canvas_press(self, event):
        items = self.canvas.find_closest(event.x, event.y)
        if not items:
            self._on_pan_press(event)
            return
        idx = self._find_object_index_from_item(items[0])
        if idx is None:
            self._on_pan_press(event)
            return
        self.dragging_index = idx
        self.drag_mode = "resize" if (event.state & 0x0001) else "move"
        obj = self.objects[idx]
        position = obj.get("position", [0.0, 0.0, 0.0])
        scale = obj.get("scale", [1.0, 1.0])
        self.drag_start = {
            "center": (float(position[0]), float(position[1])),
            "scale": (float(scale[0]), float(scale[1])),
        }
        self.selected_index = idx
        self.object_list.selection_clear(0, tk.END)
        self.object_list.selection_set(idx)
        self._load_form(self.objects[idx])
        self._redraw_canvas()

    def _on_canvas_drag(self, event):
        if self.panning and self.dragging_index is None:
            self._on_pan_drag(event)
            return
        if self.dragging_index is None:
            return
        idx = self.dragging_index
        world_x, world_y = self._canvas_to_world(event.x, event.y)
        obj = self.objects[idx]
        position = obj.get("position", [0.0, 0.0, 0.0])
        if len(position) < 3:
            position = [0.0, 0.0, 0.0]

        if self.drag_mode == "resize" and self.drag_start:
            center_x, center_y = self.drag_start["center"]
            dx = abs(world_x - center_x)
            dy = abs(world_y - center_y)
            scale_x = max(dx * 2.0, 0.05)
            scale_y = max(dy * 2.0, 0.05)
            obj["scale"] = [scale_x, scale_y]
        else:
            if self.view_vars["snap"].get():
                _, _, _, snap_size = self._get_view_settings()
                world_x = round(world_x / snap_size) * snap_size
                world_y = round(world_y / snap_size) * snap_size
            obj["position"] = [world_x, world_y, position[2]]
        self.objects[idx] = obj
        if self.selected_index == idx:
            self._load_form(obj)
        self._redraw_canvas()

    def _on_canvas_release(self, event):
        self.dragging_index = None
        self.drag_mode = None
        self.drag_start = None
        self._on_pan_release(event)

    def _on_pan_press(self, event):
        self.panning = True
        try:
            origin_x = float(self.view_vars["origin_x"].get())
            origin_y = float(self.view_vars["origin_y"].get())
        except ValueError:
            origin_x = 0.0
            origin_y = 0.0
        self.pan_start = {
            "mouse": (event.x, event.y),
            "origin": (origin_x, origin_y),
        }

    def _on_pan_drag(self, event):
        if not self.panning or not self.pan_start:
            return
        ppu, _, _, _ = self._get_view_settings()
        if ppu <= 0:
            return
        start_x, start_y = self.pan_start["mouse"]
        origin_x, origin_y = self.pan_start["origin"]
        dx = event.x - start_x
        dy = event.y - start_y
        new_origin_x = origin_x - (dx / ppu)
        new_origin_y = origin_y + (dy / ppu)
        self.view_vars["origin_x"].set(f"{new_origin_x:.3f}")
        self.view_vars["origin_y"].set(f"{new_origin_y:.3f}")
        self._redraw_canvas()

    def _on_pan_release(self, event):
        self.panning = False
        self.pan_start = None

    def _get_texture_for_size(self, texture_name, width_px, height_px):
        if not (Image and ImageTk):
            base = self.texture_sources.get(texture_name)
            if base is None:
                return self.texture_images.get(texture_name)
            if width_px <= 0 or height_px <= 0:
                return self.texture_images.get(texture_name)
            key = (texture_name, int(width_px), int(height_px))
            if key in self.texture_cache:
                return self.texture_cache[key]
            base_w = base.width()
            base_h = base.height()
            target_w = max(1, int(width_px))
            target_h = max(1, int(height_px))
            if target_w <= base_w and target_h <= base_h:
                x_factor = max(1, int(round(base_w / target_w)))
                y_factor = max(1, int(round(base_h / target_h)))
                scaled = base.subsample(x_factor, y_factor)
            elif target_w >= base_w and target_h >= base_h:
                x_factor = max(1, int(round(target_w / base_w)))
                y_factor = max(1, int(round(target_h / base_h)))
                scaled = base.zoom(x_factor, y_factor)
            else:
                scaled = base
            self.texture_cache[key] = scaled
            return scaled
        if width_px <= 0 or height_px <= 0:
            return self.texture_images.get(texture_name)
        key = (texture_name, int(width_px), int(height_px))
        if key in self.texture_cache:
            return self.texture_cache[key]
        path = os.path.join(self.textures_dir, f"{texture_name}.png")
        if not os.path.isfile(path):
            return self.texture_images.get(texture_name)
        try:
            pil_img = Image.open(path)
            size = (max(1, int(width_px)), max(1, int(height_px)))
            pil_img = pil_img.resize(size, Image.LANCZOS)
            photo = ImageTk.PhotoImage(pil_img)
            self.texture_cache[key] = photo
            return photo
        except (OSError, ValueError):
            return self.texture_images.get(texture_name)

    def _build_texture_palette(self):
        for widget in self.texture_palette.winfo_children():
            if isinstance(widget, ttk.Button):
                widget.destroy()
        for name in self.textures:
            image = self.texture_images.get(name)
            label = name if image is None else ""
            button = ttk.Button(
                self.texture_palette,
                text=label,
                image=image,
                command=lambda n=name: self._set_texture(n),
            )
            button.pack(side=tk.LEFT, padx=2)
        ttk.Button(
            self.texture_palette, text="Add Texture...", command=self._import_texture
        ).pack(side=tk.LEFT, padx=(8, 2))

    def _set_texture(self, name):
        idx = self.selected_index
        if idx is None:
            return
        obj = self.objects[idx]
        obj["texture"] = name
        self.objects[idx] = obj
        self.form_vars["texture"].set(name)
        self._refresh_list()
        self.object_list.selection_set(idx)
        self._redraw_canvas()

    def _import_texture(self):
        filename = filedialog.askopenfilename(
            title="Import Texture",
            filetypes=[("PNG files", "*.png")],
        )
        if not filename:
            return
        name = os.path.splitext(os.path.basename(filename))[0]
        dest = os.path.join(self.textures_dir, f"{name}.png")
        try:
            shutil.copy2(filename, dest)
        except OSError as exc:
            messagebox.showerror("Import Failed", f"Could not copy texture:\n{exc}")
            return
        if name not in self.textures:
            self.textures.append(name)
            self.textures.sort()
        try:
            if Image and ImageTk:
                pil_img = Image.open(dest)
                max_size = 64
                scale = max(pil_img.width / max_size, pil_img.height / max_size)
                if scale > 1:
                    size = (int(pil_img.width / scale), int(pil_img.height / scale))
                    pil_img = pil_img.resize(size, Image.LANCZOS)
                self.texture_images[name] = ImageTk.PhotoImage(pil_img)
            else:
                full_image = tk.PhotoImage(file=dest)
                self.texture_sources[name] = full_image
                max_size = 64
                scale = max(full_image.width() / max_size, full_image.height() / max_size)
                if scale > 1:
                    factor = int(scale) + 1
                    full_image = full_image.subsample(factor, factor)
                self.texture_images[name] = full_image
        except (OSError, ValueError, tk.TclError):
            self.texture_images[name] = None
        self.texture_cache = {}
        self.texture_box.configure(values=self.textures)
        self._build_texture_palette()

def main():
    root = tk.Tk()
    app = LevelEditor(root)
    root.mainloop()


if __name__ == "__main__":
    main()
