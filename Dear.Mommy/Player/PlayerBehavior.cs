using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;

public class PlayerBehavior : MonoBehaviour
{
    // 플레이어 입력을 받아오기 위한 객체들
    [SerializeField] private InputActionAsset inputActionAsset;
    private InputActionMap fieldActionMap;
    private InputAction playerMoveAction;
    private InputAction interactionAction;
    private InputAction pauseGameAction;

    // 에디터에서 조정 가능한 값들
    [SerializeField] private float playerMoveSpeed = 10f;
    [SerializeField] private float interactionRange = 1f;
    
    private Vector2 playerFaceDirection;
    private int interactiveObjectLayermask;
    private bool isMovingInAnimator;

    // 애니메이션
    private Animator animator;

    public AudioSource stepSound;  // 발소리

    void Awake()
    {
        animator = GetComponent<Animator>();
        isMovingInAnimator = false;

        // 입력 매핑
        fieldActionMap = inputActionAsset.FindActionMap("field", true);
        playerMoveAction = fieldActionMap.FindAction("PlayerMove", true);
        interactionAction = fieldActionMap.FindAction("Interaction", true);
        pauseGameAction = fieldActionMap.FindAction("PauseGame", true);

        playerFaceDirection = Vector2.down;
    }

    void Start()
    {
        interactiveObjectLayermask = LayerMask.NameToLayer("InteractiveObject");
        interactiveObjectLayermask = 1 << interactiveObjectLayermask;
        stepSound = GetComponent<AudioSource>();
    }

    void FixedUpdate()
    {
        PlayerMove();
        
    }

    public void OnEnable()
    {
        fieldActionMap.Enable();
        interactionAction.performed += Interact;
        pauseGameAction.performed += OnPauseGameInput;
    }

    public void OnDisable()
    {
        fieldActionMap.Disable();
        interactionAction.performed -= Interact;
        pauseGameAction.performed -= OnPauseGameInput;
    }

    // 이동
    void PlayerMove()
    {        
        // 이동 입력 값
        Vector2 moveDirection = playerMoveAction.ReadValue<Vector2>();


        // 이동 입력이 없다면 함수를 종료한다.
        if (moveDirection == Vector2.zero)
        {
            if (isMovingInAnimator)
            {
                animator.SetBool("IsMoving", false);
                isMovingInAnimator = false;
                stepSound.Stop();
                
            }
            return;
        }
        
        transform.position += new Vector3(moveDirection.x, moveDirection.y) * playerMoveSpeed * Time.deltaTime;
        playerFaceDirection = moveDirection;

        if (!isMovingInAnimator)
        {
            animator.SetBool("IsMoving", true);
            isMovingInAnimator = true;
            stepSound.Play();
        }
        animator.SetFloat("MoveHorizontal", moveDirection.x);
        animator.SetFloat("MoveVertical", moveDirection.y);
    }

    // 상호작용
    private void Interact(InputAction.CallbackContext context)
    {
        Debug.Log("PlayerBehavior - Interact");
        RaycastHit2D raycastHitResult = Physics2D.Raycast(transform.position, (Vector3)playerFaceDirection, interactionRange, interactiveObjectLayermask);
        Debug.DrawRay(transform.position, playerFaceDirection * interactionRange);
        
        if (raycastHitResult.collider != null)
        {
            Debug.Log($"PlayerBehavior - Interact Raycast Hit {raycastHitResult.collider.gameObject.name}");
            IInteractiveObject interactTarget = raycastHitResult.transform.gameObject.GetComponent<IInteractiveObject>();
            if (interactTarget != null)
            {
                interactTarget.OnInteract();
            }
            else
            {
                Debug.Log("interactTarget is null!");
            }
        }
    }

    private void OnPauseGameInput(InputAction.CallbackContext context)
    {
        Debug.Log("PlayerBehavior - OnPauseGameInput");
        GameManager.GetInstance().PauseGame();
    }
}
