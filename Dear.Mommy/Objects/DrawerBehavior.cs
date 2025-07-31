using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class DrawerBehavior : MonoBehaviour, IInteractiveObject
{
    [SerializeField] private bool isOpen = false;

    private SpriteRenderer spriteRenderer;
    [SerializeField] private Sprite OpenDrawerSprite;
    [SerializeField] private Sprite ClosedDrawerSprite;

    void Awake()
    {
        spriteRenderer = GetComponent<SpriteRenderer>();
        if (isOpen)
            spriteRenderer.sprite = OpenDrawerSprite;
        else
            spriteRenderer.sprite = ClosedDrawerSprite;
    }

    public void OnInteract()
    {
        isOpen = !isOpen;
        if (isOpen)
        {
            spriteRenderer.sprite = OpenDrawerSprite;
            OnOpen();
        }
        else
            spriteRenderer.sprite = ClosedDrawerSprite;
    }

    private void OnOpen()
    {
        Debug.Log("DrawerBehavior - OnOpen");
    }
}
