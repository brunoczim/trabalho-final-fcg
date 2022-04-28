SUBFACES = 16

def is_coord_at_edge(coord):
    return coord == 0 or coord == SUBFACES - 1

def is_vertex_at_corner(point):
    return all(map(is_coord_at_edge, point))

def is_vertex_at_edge(point):
    count = 0
    for coord in point:
        if not is_coord_at_edge(coord):
            count += 1
        if count > 1:
            return False
    return count <= 1

def vertex_index(point):
    for coord in point:
        if coord < 0 or coord >= SUBFACES:
            raise Exception(f'blergh: {point}')

    if is_vertex_at_corner(point):
        index = 0
        for coord in point:
            index *= 2
            if coord == SUBFACES - 1:
                index += 1
        return index
    if is_vertex_at_edge(point):
        base_index = pow(2, len(point))
        offset_i = 0
        offset = 0
        multiplier = 0
        for i, coord in enumerate(point):
            if coord == SUBFACES - 1:
                multiplier *= 2
                multiplier += 1
            elif coord == 0:
                multiplier *= 2
            else:
                offset_i = i * pow(2, len(point) - 1) * (SUBFACES - 2)
                offset = coord - 1
        return base_index + offset_i + multiplier * (SUBFACES - 2) + offset
    base_index = (
            pow(2, len(point))
            + len(point) * pow(2, len(point) - 1) * (SUBFACES - 2))
    offset = 0
    multiplier = 0
    has_axis = False
    for i, coord in enumerate(point):
        if coord == SUBFACES - 1:
            multiplier = (2 * i +  1)
            has_axis = True
        elif coord == 0:
            multiplier = (2 * i)
            has_axis = True
        else:
            offset *= SUBFACES - 2
            offset += coord - 1

    if not has_axis:
        raise Exception(f'blargh {point}')

    return (
            base_index
            + offset
            + multiplier * pow(SUBFACES - 2, 2))

vertices = dict()

for vertex_x in [0, SUBFACES - 1]:
    for vertex_y in [0, SUBFACES - 1]:
        for vertex_z in [0, SUBFACES - 1]:
            vertex = (vertex_x, vertex_y, vertex_z)
            vertices[vertex] = vertex_index(vertex)

for axis in range(3):
    for vertex_axis_2 in [0, SUBFACES - 1]:
        for vertex_axis_3 in [0, SUBFACES - 1]:
            for vertex_axis_1 in range(1, SUBFACES - 1):
                vertex_as_list = [0, 0, 0]
                vertex_as_list[axis] = vertex_axis_1
                vertex_as_list[(axis + 1) % 3] = vertex_axis_2
                vertex_as_list[(axis + 2) % 3] = vertex_axis_3
                vertex = tuple(vertex_as_list)
                vertices[vertex] = vertex_index(vertex)

for axis in range(3):
    for vertex_axis_2 in range(1, SUBFACES - 1):
        for vertex_axis_3 in range(1, SUBFACES - 1):
            for vertex_axis_1 in [0, SUBFACES - 1]:
                vertex_as_list = [0, 0, 0]
                vertex_as_list[axis] = vertex_axis_1
                vertex_as_list[(axis + 1) % 3] = vertex_axis_2
                vertex_as_list[(axis + 2) % 3] = vertex_axis_3
                vertex = tuple(vertex_as_list)
                vertices[vertex] = vertex_index(vertex)


vertices_set = dict()
for point in vertices.keys():
    index = vertices[point]
    if index in vertices_set:
        raise Exception(
                f'duplicated vertex index {index} for vertices {point} and {vertices_set[index]}')
    vertices_set[index] = point

vertices_list = list(vertices.keys())
vertices_list.sort(key=lambda vertex: vertices[vertex])

last_index = -1
for vertex in vertices_list:
    index = vertex_index(vertex)
    if index != last_index + 1:
        raise Exception(f'Bad index sequence: {last_index} -> {index}, vertex: {vertex}')
    last_index = index

faces = set()

for axis in range(3):
    for vertex_axis_2 in range(0, SUBFACES):
        for vertex_axis_3 in range(0, SUBFACES):
            for vertex_axis_1 in [0, SUBFACES - 1]:
                vertex_as_list = [0, 0, 0]
                vertex_as_list[axis] = vertex_axis_1
                vertex_as_list[(axis + 1) % 3] = vertex_axis_2
                vertex_as_list[(axis + 2) % 3] = vertex_axis_3

                vertex = tuple(vertex_as_list)

                if (vertex_axis_2 < SUBFACES - 1
                        and vertex_axis_3 < SUBFACES - 1):
                    neighbour_1_list = [0, 0, 0]
                    neighbour_1_list[axis] = vertex_axis_1
                    neighbour_1_list[(axis + 1) % 3] = vertex_axis_2
                    neighbour_1_list[(axis + 2) % 3] = vertex_axis_3 + 1

                    neighbour_2_list = [0, 0, 0]
                    neighbour_2_list[axis] = vertex_axis_1
                    neighbour_2_list[(axis + 1) % 3] = vertex_axis_2 + 1
                    neighbour_2_list[(axis + 2) % 3] = vertex_axis_3 + 1

                    neighbour_1 = tuple(neighbour_1_list)
                    neighbour_2 = tuple(neighbour_2_list)

                    faces.add((
                        vertex_index(vertex),
                        vertex_index(neighbour_2),
                        vertex_index(neighbour_1)))

                    neighbour_3_list = [0, 0, 0]
                    neighbour_3_list[axis] = vertex_axis_1
                    neighbour_3_list[(axis + 1) % 3] = vertex_axis_2 + 1
                    neighbour_3_list[(axis + 2) % 3] = vertex_axis_3

                    neighbour_4_list = [0, 0, 0]
                    neighbour_4_list[axis] = vertex_axis_1
                    neighbour_4_list[(axis + 1) % 3] = vertex_axis_2 + 1
                    neighbour_4_list[(axis + 2) % 3] = vertex_axis_3 + 1

                    neighbour_3 = tuple(neighbour_3_list)
                    neighbour_4 = tuple(neighbour_4_list)

                    faces.add((
                        vertex_index(vertex),
                        vertex_index(neighbour_4),
                        vertex_index(neighbour_3)))

print('g block')

for vertex in vertices_list:
    print('v',
            vertex[0] / (SUBFACES - 1) - 0.5,
            vertex[1] / (SUBFACES - 1) - 0.5,
            vertex[2] / (SUBFACES - 1) - 0.5)

for face in faces:
    print('f ',
            face[0] + 1, '//', face[0] + 1, ' ',
            face[1] + 1, '//', face[1] + 1, ' ',
            face[2] + 1, '//', face[2] + 1, sep='')
