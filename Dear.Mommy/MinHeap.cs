using System.Collections.Generic;
using UnityEngine;

public class MinHeap<T> where T : System.IComparable<T>
{
    private List<T> heap;

    public MinHeap(int capacity)
    {
        heap = new List<T>(capacity);
    }

    public void Add(T node)
    {
        heap.Add(node);

        int index = heap.Count - 1;
        while (index != 0)
        {
            int parent = GetParentIndex(index);

            if (heap[index].CompareTo(heap[parent]) < 0)
            {
                (heap[index], heap[parent]) = (heap[parent], heap[index]);
                index = parent;
            }
            else
                break;
        }
    }

    public T Top()
    {
        return heap[0];
    }

    public void Pop()
    {
        if (heap.Count == 0)
            return;

        heap[0] = heap[heap.Count - 1];
        heap.RemoveAt(heap.Count - 1);

        int index = 0;
        while (true)
        {
            int swapNodeIndex;
            int leftChild = GetLeftChildIndex(index);
            int rightChild = GetRightChildIndex(index);

            if (leftChild >= heap.Count)
                break;
            else if (rightChild >= heap.Count)
                swapNodeIndex = leftChild;
            else
                swapNodeIndex = heap[leftChild].CompareTo(heap[rightChild]) < 0 ? leftChild : rightChild;

            if (heap[index].CompareTo(heap[swapNodeIndex]) > 0)
            {
                (heap[index], heap[swapNodeIndex]) = (heap[swapNodeIndex], heap[index]);
                index = swapNodeIndex;
            }
            else
                break;
        }
    }

    public bool Empty()
    {
        return heap.Count == 0;
    }

    public int Size()
    {
        return heap.Count;
    }

    public void Clear()
    {
        heap.Clear();
    }

    private int GetParentIndex(int nodeIndex)
    {
        return (nodeIndex + 1) / 2 - 1;
    }

    private int GetLeftChildIndex(int nodeIndex)
    {
        return (nodeIndex + 1) * 2 - 1;
    }

    private int GetRightChildIndex(int nodeIndex)
    {
        return (nodeIndex + 1) * 2;
    }
}