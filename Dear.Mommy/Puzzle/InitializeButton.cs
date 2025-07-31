using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class InitializeButton : MonoBehaviour, IInteractiveObject
{
    private bool isPressed;
    [SerializeField] private float buttonPressDelay = 1.0f;
    
    private SpriteRenderer spriteRenderer;
    [SerializeField] Sprite buttonNotPressedSprite;
    [SerializeField] Sprite buttonPressedSprite;

    private TileQuizManager tileQuizManager;

    void Awake()
    {        
        spriteRenderer = GetComponent<SpriteRenderer>();
    }

    void Start()
    {
        isPressed = false;
        tileQuizManager = GameObject.FindAnyObjectByType<TileQuizManager>();
    }

    public void OnInteract()
    {
        if (!isPressed)
        {
            isPressed = true;
            spriteRenderer.sprite = buttonPressedSprite;
            tileQuizManager.Initialize();
            Invoke("RestoreButton", buttonPressDelay);
        }
    }

    private void RestoreButton()
    {
        isPressed = false;
        spriteRenderer.sprite = buttonNotPressedSprite;
    }
}
