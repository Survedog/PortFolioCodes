using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class DoorBehavior : MonoBehaviour, IInteractiveObject
{
    [SerializeField] protected bool isOpen = false;

    protected SpriteRenderer spriteRenderer;
    [SerializeField] protected Sprite openDoorSprite;
    [SerializeField] protected Sprite closedDoorSprite;
    [SerializeField] protected bool loadSceneWhenOpen = false; 
    [SerializeField] protected string sceneToLoadOnOpen;

    [SerializeField] protected AudioSource doorOpenSound;
    [SerializeField] protected AudioSource doorCloseSound;
    
    [SerializeField] protected int spawnPointIndex = 0;

    protected virtual void Awake()
    {
        spriteRenderer = GetComponent<SpriteRenderer>();
        if (isOpen)
            spriteRenderer.sprite = openDoorSprite;
        else
            spriteRenderer.sprite = closedDoorSprite;
    }

    public virtual void OnInteract()
    {
        isOpen = !isOpen;
        if (isOpen)
        {
            spriteRenderer.sprite = openDoorSprite;
            doorOpenSound.Play();
            if (loadSceneWhenOpen && !string.IsNullOrEmpty(sceneToLoadOnOpen))
                GameManager.GetInstance().LoadScene(sceneToLoadOnOpen, spawnPointIndex);
        }
        else
        {
            spriteRenderer.sprite = closedDoorSprite;
            doorCloseSound.Play();
        }
    }
}
