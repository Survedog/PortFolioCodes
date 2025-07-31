using UnityEngine;
using System.Collections.Generic;
using UnityEngine.SceneManagement;

public class MonsterBehavior : MonoBehaviour
{
    [SerializeField] private Transform chaseTarget;
    [SerializeField] private float detectRange = 15f;
    [SerializeField] private float moveSpeed = 10f;
    [SerializeField] private float pathNearThreshold = 0.5f;
    [SerializeField] private float findPathCoolTime = 0.5f;
    [SerializeField] private float gameOverReloadSceneDelay = 3f;

    private float remainingFindPathCoolTime;
    private Stack<Vector3> chasePathStack;
    private Vector3 nextPathNode;
    private Vector3 monsterPathOriginOffset = new Vector3(0f, -1f), playerPathOriginOffset = new Vector3(0f, -0.7f);

    private Animator animator;
    private bool isMovingInAnimator;
    private bool isChasing;

    private DialogManager dialogManager;
    [SerializeField] private GameObject gameOverDialogPanel;

    public AudioSource stepSound;
    public AudioSource srcSound;
    [SerializeField] private AudioSource laughSound;

    void Awake()
    {
        animator = GetComponent<Animator>();
        chasePathStack = new Stack<Vector3>();
        dialogManager = FindAnyObjectByType<DialogManager>();
    }

    void Start()
    {
        isChasing = true;
        GameManager.GetInstance().isMonsterChasingPlayer = true;

        isMovingInAnimator = false;
        stepSound = GetComponent<AudioSource>();
        srcSound = GetComponent<AudioSource>();
    }

    void FixedUpdate()
    {
        if (isChasing)
        {
            if (!isMovingInAnimator)
            {
                animator.SetBool("IsMoving", true);
                isMovingInAnimator = true;
                stepSound.Play();
            }
            FindPathToTarget();
            MoveToTarget();
        }
        else
        {
            if (isMovingInAnimator)
            {
                animator.SetBool("IsMoving", false);
                isMovingInAnimator = false;
                stepSound.Stop();
            }
        }
    }

    void FindPathToTarget()
    {
        remainingFindPathCoolTime -= Time.deltaTime;
        if (remainingFindPathCoolTime <= 0f)
        {
            remainingFindPathCoolTime = findPathCoolTime;
            Pathfinding.GetInstance().FindPath(transform.position + monsterPathOriginOffset, chaseTarget.position + playerPathOriginOffset, chasePathStack);

            Pathfinding.DrawPathDebugLine(new Stack<Vector3>(chasePathStack));

            if (chasePathStack.Count != 0)
                nextPathNode = chasePathStack.Pop();
        }
    }

    void MoveToTarget()
    {
        Vector3 moveVector = chaseTarget.position - (transform.position + monsterPathOriginOffset);

        if (moveVector.magnitude > 2.0f)
        {
            if (chasePathStack.Count == 0)
                return;

            moveVector = nextPathNode - (transform.position + monsterPathOriginOffset);
            if (moveVector.magnitude < pathNearThreshold)
            {
                nextPathNode = chasePathStack.Pop();
                moveVector = nextPathNode - (transform.position + monsterPathOriginOffset);
            }
        }

        transform.position += moveVector.normalized * moveSpeed * Time.fixedDeltaTime;

        animator.SetFloat("MoveHorizontal", moveVector.x);
        animator.SetFloat("MoveVertical", moveVector.y);
    }

    void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.CompareTag("Player"))
        {
            Debug.Log("MonsterBehavior - CaughtPlayer");
            srcSound.Stop();
            laughSound.Play();
            dialogManager.ShowDialogNoTextUnableAction(gameOverDialogPanel,
                () => Invoke("ReloadCurrentScene", gameOverReloadSceneDelay));
        }
    }

    void ReloadCurrentScene()
    {
        Debug.Log($"ReloadCurrentScene - {SceneManager.GetActiveScene().name}");
        GameManager.GetInstance().LoadScene(SceneManager.GetActiveScene().name);
    }
}