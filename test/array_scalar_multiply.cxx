#include "array_scalar_multiply.h"

#include "array.h"

#include <iostream>
#include <sstream>

using std::vector;
using std::string;
using std::ostringstream;
using std::cerr;
using std::endl;

// --------------------------------------------------------------------------
array_scalar_multiply::array_scalar_multiply() : scalar(0)
{
    this->set_number_of_inputs(1);
    this->set_number_of_outputs(1);
}

// --------------------------------------------------------------------------
array_scalar_multiply::~array_scalar_multiply()
{}

// --------------------------------------------------------------------------
int array_scalar_multiply::get_active_array(
    const teca_meta_data &input_md,
    std::string &active_array)
{
    if (this->array_name.empty())
    {
        // by default process the first array found on the input
        if (!input_md.has("array_names"))
        {
            TECA_ERROR("no array specified and none found on the input")
            return -1;
        }

        vector<string> array_names;
        input_md.get_prop("array_names", array_names);

        active_array = array_names[0];
    }
    else
    {
        // otherwise process the requested array
        active_array = this->array_name;
    }
    return 0;
}

// --------------------------------------------------------------------------
teca_meta_data array_scalar_multiply::get_output_meta_data(
    unsigned int port,
    std::vector<teca_meta_data> &input_md)
{
    cerr << "array_scalar_multiply::get_output_meta_data" << endl;

    teca_meta_data output_md(input_md[0]);

    // if the user has requested a specific array then
    // replace "array_names" in the output metadata.
    // otherwise pass through and rely on down stream
    // requests to select the array to process
    if (this->array_name.size())
        output_md.set_prop("array_names", this->array_name);

    return output_md;
}

// --------------------------------------------------------------------------
std::vector<teca_meta_data> array_scalar_multiply::get_upstream_request(
    unsigned int port,
    std::vector<teca_meta_data> &input_md,
    teca_meta_data &request)
{
    cerr << "array_scalar_multiply::get_upstream_request" << endl;

    vector<teca_meta_data> up_reqs;

    // get the active array from the incoming request
    string active_array;
    if (request.get_prop("array_name", active_array))
    {
        TECA_ERROR("array_name is not set on incoming the request")
        return up_reqs;
    }

    teca_meta_data up_req(request);
    up_req.set_prop("array_name", active_array);

    up_reqs.push_back(up_req);
    return up_reqs;
}

// --------------------------------------------------------------------------
p_teca_dataset array_scalar_multiply::execute(
    unsigned int port,
    std::vector<p_teca_dataset> &input_data,
    teca_meta_data &request)
{
    cerr << "array_scalar_multiply::execute" << endl;

    p_array a_in = std::dynamic_pointer_cast<array>(input_data[0]);
    if (!a_in)
    {
        TECA_ERROR("no array to process")
        return p_teca_dataset();
    }

    p_array a_out = array::New();
    a_out->copy_structure(a_in);

    std::transform(
        a_in->get_data().begin(),
        a_in->get_data().end(),
        a_out->get_data().begin(),
        [this](double d) -> double { return d * this->scalar; });

    return a_out;
}