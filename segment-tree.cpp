#include <bits/stdc++.h>
using namespace std;

class SegmentTree {
private:
    int n;
    vector<double> tree_sum;      // Suma de valores
    vector<double> tree_sum2;     // Suma de cuadrados
    vector<double> lazy;         // Lazy: valor para asignar (-1 significa sin lazy)
    vector<bool> has_lazy;

    // Merging de dos nodos hijos
    void merge(int node, int left_child, int right_child) {
        tree_sum[node] = tree_sum[left_child] + tree_sum[right_child];
        tree_sum2[node] = tree_sum2[left_child] + tree_sum2[right_child];
    }

    // Aplicar lazy a un nodo
    void apply_lazy(int node, int start, int end) {
        if (has_lazy[node]) {
            double val = lazy[node];
            int len = end - start + 1;

            tree_sum[node] = val * len;
            tree_sum2[node] = val * val * len;

            if (start != end) {
                // Propagar a hijos
                lazy[2*node] = val;
                has_lazy[2*node] = true;
                lazy[2*node+1] = val;
                has_lazy[2*node+1] = true;
            }

            has_lazy[node] = false;
        }
    }

    // Propagar lazy hacia abajo
    void propagate(int node, int start, int end) {
        if (has_lazy[node]) {
            apply_lazy(node, start, end);
        }
    }

    void build(int node, int start, int end, const vector<double>& arr) {
        if (start == end) {
            tree_sum[node] = arr[start];
            tree_sum2[node] = arr[start] * arr[start];
            return;
        }
        int mid = (start + end) / 2;
        build(2*node, start, mid, arr);
        build(2*node+1, mid+1, end, arr);
        merge(node, 2*node, 2*node+1);
    }

    void update_range(int node, int start, int end, int l, int r, double val) {
        propagate(node, start, end);

        if (start > end || start > r || end < l) return;

        if (start >= l && end <= r) {
            lazy[node] = val;
            has_lazy[node] = true;
            propagate(node, start, end);
            return;
        }

        int mid = (start + end) / 2;
        update_range(2*node, start, mid, l, r, val);
        update_range(2*node+1, mid+1, end, l, r, val);
        merge(node, 2*node, 2*node+1);
    }

    pair<double, double> query(int node, int start, int end, int l, int r) {
        propagate(node, start, end);

        if (start > end || start > r || end < l)
            return {0.0, 0.0};

        if (start >= l && end <= r) {
            return {tree_sum[node], tree_sum2[node]};
        }

        int mid = (start + end) / 2;
        auto left = query(2*node, start, mid, l, r);
        auto right = query(2*node+1, mid+1, end, l, r);

        return {left.first + right.first, left.second + right.second};
    }

public:
    SegmentTree(int size, const vector<double>& arr) : n(size) {
        tree_sum.assign(4*n, 0.0);
        tree_sum2.assign(4*n, 0.0);
        lazy.assign(4*n, 0.0);
        has_lazy.assign(4*n, false);
        build(1, 0, n-1, arr);
    }

    // Asignar valor a rango [l, r]
    void update(int l, int r, double val) {
        update_range(1, 0, n-1, l, r, val);
    }

    // Obtener varianza en rango [l, r]
    double get_variance(int l, int r) {
        auto [sum, sum2] = query(1, 0, n-1, l, r);
        int len = r - l + 1;
        if (len == 1) return 0.0;
        double mean = sum / len;
        double variance = (sum2 / len) - (mean * mean);
        return variance;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout << fixed << setprecision(6);

    int n, q;
    cin >> n >> q;
    vector<double> arr(n);
    for (int i = 0; i < n; i++) {
        cin >> arr[i];
    }

    SegmentTree st(n, arr);

    while (q--) {
        int type;
        cin >> type;
        if (type == 1) {
            // Update: 1 l r val
            int l, r;
            double val;
            cin >> l >> r >> val;
            st.update(l, r, val);
        } else if (type == 2) {
            // Query: 2 l r
            int l, r;
            cin >> l >> r;
            double var = st.get_variance(l, r);
            cout << var << "\n";
        }
    }

    return 0;
}