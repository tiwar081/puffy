import cv2
import numpy as np
from PIL import Image


def add_left_padding(image, padding_width, padding_color=(255, 255, 255)):
    """
    Add padding to the left side of an image.
    
    Args:
        image: Input image (numpy array)
        padding_width: Width of padding to add (in pixels)
        padding_color: Color of the padding (BGR format for OpenCV)
    
    Returns:
        Padded image
    """
    h, w = image.shape[:2]
    padded_image = np.full((h, w + padding_width, 3), padding_color, dtype=np.uint8)
    padded_image[:, padding_width:] = image
    return padded_image


def interactive_generate_mask(source_path: str, mask_save_path: str, output_size=(256, 256), left_padding=0) -> None:
    """
    Interactively draw a mask on top of the input image and save a 256x256 binary mask.

    - Left mouse: freehand draw a polygon; on release, it is filled into the mask
    - s: save mask and exit
    - r: reset/clear mask
    - q or ESC: quit without saving
    
    Args:
        source_path: Path to the source image
        mask_save_path: Path to save the generated mask
        output_size: Target size for the output image (width, height)
        left_padding: Width of padding to add to the left side before resizing
    """

    # Load source image
    source = cv2.imread(source_path, cv2.IMREAD_COLOR)
    if source is None:
        raise FileNotFoundError(f"Could not read image: {source_path}")

    # Add left padding if specified
    if left_padding > 0:
        source = add_left_padding(source, left_padding)
        print(f"Added {left_padding} pixels of padding to the left side")

    # Resize to the desired output size
    target_w, target_h = output_size
    source_resized = cv2.resize(source, (target_w, target_h), interpolation=cv2.INTER_AREA)
    
    # Prepare input image save path
    input_save_path = mask_save_path.replace('mask_', 'input_')
    print(f"Debug: mask_save_path = {mask_save_path}")
    print(f"Debug: input_save_path = {input_save_path}")

    # Binary mask (single-channel), same spatial size
    mask = np.zeros((target_h, target_w), dtype=np.uint8)

    # State for current freehand polygon
    points = []
    drawing = False
    
    # History of drawn polygons for undo functionality
    polygon_history = []

    def draw_mask(event, x, y, flags, param):
        nonlocal drawing, points, mask, polygon_history
        if event == cv2.EVENT_LBUTTONDOWN:
            drawing = True
            points = [(x, y)]
        elif event == cv2.EVENT_MOUSEMOVE and drawing:
            points.append((x, y))
        elif event == cv2.EVENT_LBUTTONUP:
            drawing = False
            points.append((x, y))
            if len(points) >= 3:
                polygon = np.array(points, dtype=np.int32)
                cv2.fillPoly(mask, [polygon], 255)
                # Store the polygon for undo functionality
                polygon_history.append(polygon)
            points = []

    window_name = 'Draw Mask'
    cv2.namedWindow(window_name, cv2.WINDOW_AUTOSIZE)
    cv2.setMouseCallback(window_name, draw_mask)

    print("Instructions: draw with left mouse; 's' save, 'r' reset, 'b' undo, 'q' quit")

    while True:
        # Start from the original image
        display = source_resized.copy()

        # Overlay the current mask with transparency
        if np.any(mask):
            overlay = display.copy()
            green = np.array([0, 255, 0], dtype=np.uint8)
            overlay[mask > 0] = (
                0.5 * overlay[mask > 0] + 0.5 * green
            ).astype(np.uint8)
            display = overlay

        # Draw the current in-progress polyline
        if len(points) >= 2:
            for i in range(1, len(points)):
                cv2.line(display, points[i - 1], points[i], (255, 255, 255), 1)

        cv2.imshow(window_name, display)
        key = cv2.waitKey(10) & 0xFF

        if key == ord('s'):
            # Save single-channel 0/255 mask
            success_mask = cv2.imwrite(mask_save_path, mask)
            print(f"Mask saved to {mask_save_path} (success: {success_mask})")
            
            # Save the resized input image using PIL
            print(f"Attempting to save input image to: {input_save_path}")
            try:
                # Convert BGR to RGB for PIL
                source_rgb = cv2.cvtColor(source_resized, cv2.COLOR_BGR2RGB)
                pil_image = Image.fromarray(source_rgb)
                pil_image.save(input_save_path)
                print(f"Resized input image saved to {input_save_path} (success: True)")
            except Exception as e:
                print(f"Error saving input image: {e}")
                # Fallback to OpenCV
                success_input = cv2.imwrite(input_save_path, source_resized)
                print(f"OpenCV fallback result: {success_input}")
            break
        elif key == ord('r'):
            mask[:] = 0
            points = []
            polygon_history.clear()
            print("Mask reset")
        elif key == ord('b'):
            # Undo the last drawn polygon
            if polygon_history:
                # Clear the mask and redraw all polygons except the last one
                mask[:] = 0
                for polygon in polygon_history[:-1]:
                    cv2.fillPoly(mask, [polygon], 255)
                # Remove the last polygon from history
                removed_polygon = polygon_history.pop()
                print("Undid last polygon")
            else:
                print("No polygons to undo")
        elif key == ord('q') or key == 27:  # 'q' or ESC
            print("User cancelled drawing")
            break

    cv2.destroyAllWindows()
