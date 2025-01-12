#include <tbb/tbb.h>
#include <iostream>
#include <tuple>

int main()
{
    tbb::flow::graph g;

    tbb::flow::function_node<int, int> my_node(g, tbb::flow::unlimited, [](const int& in) {
        std::cout << "my_node message: " << in << std::endl;
        return in;
        });

    tbb::flow::function_node<int, int> my_other_node(g, tbb::flow::unlimited, [](const int& in) {
        std::cout << "my_other_node message: " << in << std::endl;
        return in;
        });

    tbb::flow::join_node<std::tuple<int, int>, tbb::flow::queueing> my_join_node(g);

    tbb::flow::function_node<std::tuple<int, int>, void> my_final_node(g, tbb::flow::unlimited, [](const std::tuple<int, int>& in) {
        std::cout << "my_final_node message: " << std::get<0>(in) << " " << std::get<1>(in) << std::endl;
        });

    tbb::flow::make_edge(tbb::flow::output_port<0>(my_node), tbb::flow::input_port<0>(my_join_node));
    tbb::flow::make_edge(tbb::flow::output_port<0>(my_other_node), tbb::flow::input_port<1>(my_join_node));
    tbb::flow::make_edge(my_join_node, my_final_node);

    my_node.try_put(13);
    my_other_node.try_put(1);

    g.wait_for_all();

    return 0;
}