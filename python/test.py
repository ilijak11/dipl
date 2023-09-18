import pygame
import sys

# Initialize pygame
pygame.init()

# Constants
WINDOW_WIDTH = 400
WINDOW_HEIGHT = 300
BACKGROUND_COLOR = (255, 255, 255)
TEXT_COLOR = (0, 0, 0)
BUTTON_COLOR = (50, 150, 255)
BUTTON_TEXT_COLOR = (255, 255, 255)
TEXT_AREA_COLOR = (220, 220, 220)
FONT = pygame.font.Font(None, 36)

# Create the window
window = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
pygame.display.set_caption("Pygame Textarea and Buttons")

# Textarea
textarea_rect = pygame.Rect(50, 50, 300, 100)
textarea_text = ""
textarea_active = False

# Buttons
button_rects = [
    pygame.Rect(50, 200, 80, 40),
    pygame.Rect(150, 200, 80, 40),
    pygame.Rect(250, 200, 80, 40),
    pygame.Rect(350, 200, 80, 40),
]
button_texts = ["Button 1", "Button 2", "Button 3", "Button 4"]

# Game loop
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.MOUSEBUTTONDOWN:
            if textarea_rect.collidepoint(event.pos):
                textarea_active = True
            else:
                textarea_active = False
        elif event.type == pygame.KEYDOWN:
            if textarea_active:
                if event.key == pygame.K_RETURN:
                    # Handle Enter key press if needed
                    pass
                elif event.key == pygame.K_BACKSPACE:
                    textarea_text = textarea_text[:-1]
                else:
                    textarea_text += event.unicode

    # Clear the screen
    window.fill(BACKGROUND_COLOR)

    # Draw textarea
    pygame.draw.rect(window, TEXT_AREA_COLOR, textarea_rect)
    text_surface = FONT.render(textarea_text, True, TEXT_COLOR)
    window.blit(text_surface, (textarea_rect.x + 10, textarea_rect.y + 10))

    # Draw buttons
    for i in range(len(button_rects)):
        pygame.draw.rect(window, BUTTON_COLOR, button_rects[i])
        button_text = FONT.render(button_texts[i], True, BUTTON_TEXT_COLOR)
        text_x = button_rects[i].centerx - button_text.get_width() // 2
        text_y = button_rects[i].centery - button_text.get_height() // 2
        window.blit(button_text, (text_x, text_y))

    pygame.display.flip()

# Quit pygame
pygame.quit()
sys.exit()