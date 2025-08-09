#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <random>
#include <cmath>

struct User {
    double spend = 0.0;
    double clicks = 0.0;
    double age = 0.0;
    double income = 0.0;
    double engagement = 0.0;
    int cluster = -1;
};

struct AdCluster {
    double avg_spend = 0.0;
    double avg_ctr = 0.0;
    double revenue_potential = 0.0;
    double bid_price = 0.0;
    std::string profile;
    std::vector<int> user_ids;
};

class AdKMeans {
private:
    std::vector<User> users;
    std::vector<AdCluster> clusters;
    int k = 5;
    std::mt19937 rng;
    
public:
    AdKMeans() : rng(std::random_device{}()) {}
    
    void load_users(const std::vector<User>& data) {
        users = data;
    }
    
    void cluster() {
        init_clusters();
        for (int iter = 0; iter < 100; ++iter) {
            assign_users();
            update_clusters();
            calculate_revenue();
        }
    }
    
    void init_clusters() {
        clusters.resize(k);
        for (int i = 0; i < k; ++i) {
            clusters[i].avg_spend = rng() % 1000;
            clusters[i].avg_ctr = (rng() % 100) / 1000.0;
        }
    }
    
    void assign_users() {
        for (int i = 0; i < users.size(); ++i) {
            double min_dist = 1e9;
            int best_cluster = 0;
            
            for (int c = 0; c < k; ++c) {
                double dist = distance(users[i], c);
                if (dist < min_dist) {
                    min_dist = dist;
                    best_cluster = c;
                }
            }
            users[i].cluster = best_cluster;
        }
    }
    
    void update_clusters() {
        for (int c = 0; c < k; ++c) {
            clusters[c].user_ids.clear();
            double total_spend = 0, total_clicks = 0;
            int count = 0;
            
            for (int i = 0; i < users.size(); ++i) {
                if (users[i].cluster == c) {
                    clusters[c].user_ids.push_back(i);
                    total_spend += users[i].spend;
                    total_clicks += users[i].clicks;
                    count++;
                }
            }
            
            if (count > 0) {
                clusters[c].avg_spend = total_spend / count;
                clusters[c].avg_ctr = total_clicks / count;
            }
        }
    }
    
    void calculate_revenue() {
        for (int c = 0; c < k; ++c) {
            double users_count = clusters[c].user_ids.size();
            clusters[c].revenue_potential = clusters[c].avg_spend * clusters[c].avg_ctr * users_count * 365;
            clusters[c].bid_price = clusters[c].avg_spend * 0.8;
            
            if (clusters[c].avg_spend > 500) clusters[c].profile = "premium";
            else if (clusters[c].avg_spend > 100) clusters[c].profile = "standard";
            else clusters[c].profile = "budget";
        }
    }
    
    double distance(const User& user, int cluster_id) {
        double spend_diff = user.spend - clusters[cluster_id].avg_spend;
        double ctr_diff = user.clicks - clusters[cluster_id].avg_ctr;
        return sqrt(spend_diff * spend_diff + ctr_diff * ctr_diff * 10000);
    }
    
    std::vector<AdCluster> get_clusters() { return clusters; }
    
    double total_revenue() {
        double total = 0;
        for (const auto& cluster : clusters) {
            total += cluster.revenue_potential;
        }
        return total;
    }
    
    void optimize_bids() {
        std::sort(clusters.begin(), clusters.end(), [](const AdCluster& a, const AdCluster& b) {
            return a.revenue_potential > b.revenue_potential;
        });
        
        for (int i = 0; i < clusters.size(); ++i) {
            clusters[i].bid_price *= (1.0 + i * 0.1);
        }
    }
};

class AdStrategy {
private:
    AdKMeans kmeans;
    
public:
    void segment_audience(const std::vector<User>& users) {
        kmeans.load_users(users);
        kmeans.cluster();
        kmeans.optimize_bids();
    }
    
    void print_strategy() {
        auto clusters = kmeans.get_clusters();
        
        for (int i = 0; i < clusters.size(); ++i) {
            std::cout << "Cluster " << i << ": " << clusters[i].profile << std::endl;
            std::cout << "Users: " << clusters[i].user_ids.size() << std::endl;
            std::cout << "Avg Spend: $" << clusters[i].avg_spend << std::endl;
            std::cout << "CTR: " << clusters[i].avg_ctr * 100 << "%" << std::endl;
            std::cout << "Bid Price: $" << clusters[i].bid_price << std::endl;
            std::cout << "Revenue: $" << clusters[i].revenue_potential << std::endl;
            std::cout << std::endl;
        }
        
        std::cout << "Total Revenue: $" << kmeans.total_revenue() << std::endl;
    }
    
    double get_roi() {
        return kmeans.total_revenue();
    }
};

int main() {
    std::vector<User> users;
    std::mt19937 rng(42);
    
    for (int i = 0; i < 1000; ++i) {
        User u;
        u.spend = 10 + (rng() % 1000);
        u.clicks = (rng() % 100) / 1000.0;
        u.age = 18 + (rng() % 60);
        u.income = 20000 + (rng() % 80000);
        u.engagement = (rng() % 100) / 100.0;
        users.push_back(u);
    }
    
    AdStrategy strategy;
    strategy.segment_audience(users);
    strategy.print_strategy();
    
    return 0;
}