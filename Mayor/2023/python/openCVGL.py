import cv2
import pygame
from pygame.locals import *

# Dimensiones de la ventana de Pygame
window_width = 800
window_height = 600

# Inicializar Pygame
pygame.init()
screen = pygame.display.set_mode((window_width, window_height))
clock = pygame.time.Clock()

cap = cv2.VideoCapture(0)  

while True:
    ret, frame = cap.read()
    
    frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    frame_surface = pygame.surfarray.make_surface(frame_rgb)
    
    # Redimensionar el frame_surface para que se ajuste a la ventana de Pygame
    frame_surface = pygame.transform.scale(frame_surface, (window_width, window_height))
    
    # Dibujar el frame en la ventana de Pygame
    screen.blit(frame_surface, (0, 0))
    pygame.display.flip()
    
    # Manejar eventos de Pygame
    event = pygame.event.poll()
    if event.type == QUIT or (event.type == KEYDOWN and event.key == K_ESCAPE):
        pygame.quit()
        break  
    
    clock.tick(30)  # Limitar la velocidad de fotogramas a 30 FPS

# Liberar recursos y cerrar la ventana de Pygame
cap.release()
pygame.quit()